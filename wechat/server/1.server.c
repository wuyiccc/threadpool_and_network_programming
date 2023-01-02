/**
 * @author: wuyiccc
 * @date: 2023-01-01 11:29
 */

#include "head.h"

WINDOW *msg_win, *sub_msg_win, *info_win, *sub_info_win, *input_win, *sub_input_win;
// 基于当前目录(程序运行目录)的相对路径
const char *config = "./wechatd.conf";
struct wechat_user *users;

int main(int argc, char **argv) {
  // 1. 读取命令行参数/或者配置文件的的port设置
  int opt;
  int port = 0;
  // 读取命令行-p参数的值
  while ((opt = getopt(argc, argv, "p:")) != -1) {
    switch (opt) {
      case 'p':
        port = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Usage : %s -p port.\n", argv[0]);
        exit(1);
    }
  }

  if (access(config, R_OK)) {
    fprintf(stderr, RED "<Error>" NONE "config file error!\n");
    exit(1);
  }

  if (!port) {
    port = atoi(get_conf_value(config, "PORT"));
    DBG("port = %d!\n", port);
  }
  DBG(YELLOW "<D>" NONE " : config file read success.\n");

  #ifdef UI
  setlocale(LC_ALL, "");
  init_ui();
  #endif

  // 2. 设置socket监听指定端口
  int server_listen;
  if ((server_listen = socket_create(port)) < 0) {
    perror("socket_create");
    exit(1);
  }

  DBG(YELLOW "<D>" NONE " : server_listen is listening on port %d.\n", port);

  // 初始化用户资源
  users = (struct wechat_user *)calloc(MAXUSERS, sizeof(struct wechat_user));

  int sockfd;
  int epollfd;
  int subefd1;
  int subefd2;

  if ((epollfd = epoll_create(1)) < 0) {
    perror("epoll_create");
    exit(1);
  }

  if ((subefd1 = epoll_create(1)) < 0) {
    perror("epoll_create");
    exit(1);
  }

  if ((subefd2 = epoll_create(1)) < 0) {
    perror("epoll_create");
    exit(1);
  }
  DBG(YELLOW "<D>" NONE " : Main reactor and two sub reactors created.\n");

  // 3. 每个线程绑定指定的从epoll
  pthread_t tid1, tid2;
  pthread_create(&tid1, NULL, sub_reactor, (void *)&subefd1);
  pthread_create(&tid1, NULL, sub_reactor, (void *)&subefd2);
  DBG(YELLOW "<D>" NONE " : Two sub reactor thread created.\n");

  struct epoll_event events[MAXEVENTS], ev;
  ev.data.fd = server_listen;
  ev.events = EPOLLIN;

  // 4. 设置主epoll监听指定的server_listen
  epoll_ctl(epollfd, EPOLL_CTL_ADD, server_listen, &ev);

  for (;;) {
    int nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
    if (nfds < 0) {
      perror("epoll_wait");
      exit(1);
    }

    for (int i = 0; i < nfds; i++) {
      int fd = events[i].data.fd;
      if (fd == server_listen && events[i].events & EPOLLIN) {
        if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
          perror("accept");
          exit(1);
        }
        ev.data.fd = sockfd;
        ev.events = EPOLLIN;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev);
      } else {
        // 接收数据, 验证数据, 完成具体操作
        struct wechat_msg msg;
        bzero(&msg, sizeof(msg));
        int ret = recv(fd, (void *)&msg, sizeof(msg), 0);

        if (ret <= 0) {
          epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
          close(fd);
          continue;
        }
        if (ret != sizeof(msg)) {
          DBG(RED "<MsgErr>" NONE " : msg size err!\n");
          continue;
        }
        if (msg.type & WECHAT_SIGUP) {
          // 注册用户, 更新用户信息到文件中, 判断是否可以注册
          msg.type = WECHAT_ACK;
          send(fd, (void *)&msg, sizeof(msg), 0);
        } else if (msg.type & WECHAT_SIGIN) {
          // 登录, 判断密码是否正确, 验证用户是否重复登录
          // 加到从反应堆中
          msg.type = WECHAT_ACK;
          send(fd, (void *)&msg, sizeof(msg), 0);
          strcpy(msg.msg, "你已登录成功!\n");
          show_msg(&msg);
          strcpy(users[fd].name, msg.from);
          users[fd].fd = fd;
          users[fd].isOnline = 1;
          users[fd].sex = msg.sex;
          int which = msg.sex ? subefd1 : subefd2;
          // 从主反应堆剔除fd
          epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
          add_to_reactor(which, fd);
        } else {
          // 报文数据有误
        }
      }
    }
  }

  sleep(10);

  return 0;
}
