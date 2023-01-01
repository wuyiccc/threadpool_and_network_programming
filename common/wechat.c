/**
 * @author: wuyiccc
 * @date: 2023-01-01 17:19
 */

#include "head.h"

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

void *sub_reactor(void *arg) {
  int subfd = *(int *)arg;
  while (1) {
    DBG(L_RED"<Sub Reactor>"NONE" : in sub reactor %d.\n", subfd);
    sleep(30);
  }
}
