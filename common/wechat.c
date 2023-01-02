/**
 * @author: wuyiccc
 * @date: 2023-01-01 17:19
 */

#include "wechat.h"

#include "head.h"

extern struct wechat_user *users;

int add_to_reactor(int epollfd, int fd) {
  struct epoll_event ev;
  ev.data.fd = fd;
  ev.events = EPOLLIN | EPOLLET;
  make_nonblock(fd);

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
    return -1;
  }
  return 0;
}

void *client_recv(void *arg) {
  int sockfd = *(int *)arg;
  struct wechat_msg msg;
  while (1) {
    bzero(&msg, sizeof(msg));
    int ret = recv(sockfd, &msg, sizeof(msg), 0);
    printf("%s : %s\n", msg.from, msg.msg);
  }
}

void *sub_reactor(void *arg) {
  int subfd = *(int *)arg;
  DBG(L_RED "<Sub Reactor>" NONE " : in sub reactor %d.\n", subfd);
  struct epoll_event ev, events[MAXEVENTS];
  for (;;) {
    // -1 代表永久无期限阻塞
    int nfds = epoll_wait(subfd, events, MAXEVENTS, -1);
    if (nfds < 0) {
      DBG(L_RED "<Sub Reactor Err> : sub reactor error %d.\n" NONE, subfd);
      continue;
    }
    for (int i = 0; i < nfds; i++) {
      int fd = events[i].data.fd;
      struct wechat_msg msg;
      bzero(&msg, sizeof(msg));
      int ret = recv(fd, (void *)&msg, sizeof(msg), 0);
      if (ret <= 0 && errno != EAGAIN) {
        // 异常情况
        close(fd);
        epoll_ctl(subfd, EPOLL_CTL_DEL, fd, NULL);
        users[fd].isOnline = 0;
        DBG(L_RED"<Sub Reactor Err> : connection of %d on %d is closed.\n"NONE, fd, subfd);
        continue;
      }
      if (ret != sizeof(msg)) {
        // 如果收到的消息大小与期望大小不一致
        DBG(L_RED"<Sub Reactor Err> : msg size err.\n"NONE);
        continue;
      }
      if (msg.type & WECHAT_WALL) {
        DBG(BLUE"%s : %s\n", msg.from, msg.msg);
      }
    }
  }
}