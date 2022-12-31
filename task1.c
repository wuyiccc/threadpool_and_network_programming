/**
 * @author: wuyiccc
 * @date: 2022-12-30 22:43
 */

#include "./common/head.h"

#define QUEUESIZE 100
#define INS 1
#define MAXEVENTS 5
#define MAXCLIENTS 2000
// 存储客户端
int clients[MAXCLIENTS];
// 给每一个客户端流一个空间来存储临时数据
char *data[MAXCLIENTS];
pthread_mutex_t mutex[MAXCLIENTS];

// 定义全局变量, 方便其他位置调用
int epollfd;

int main(int argc, char **argv) {
  int opt;
  int port = 7777;
  int server_listen;
  // 1. 获取-p 参数后面指定的值: 端口号
  while ((opt = getopt(argc, argv, "p:")) != -1) {
    switch (opt) {
      case 'p':
        port = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Usage : %s -p port!\n", argv[0]);
        exit(1);
    }
  }
  DBG(YELLOW "<D> : Server will listen on port [%d].\n" NONE, port);

  // 2. 设置server监听指定端口
  if ((server_listen = socket_create(port)) < 0) {
    perror("socket_create");
    exit(1);
  }
  DBG(YELLOW "<D> : Server_listen starts.\n" NONE);

  for (int i = 0; i < MAXCLIENTS; i++) {
    pthread_mutex_init(&mutex[i], NULL);
  }
  struct task_queue *taskQueue =
      (struct task_queue *)calloc(1, sizeof(struct task_queue));
  task_queue_init(taskQueue, QUEUESIZE);

  DBG(YELLOW "<D> : task queue init successfully.\n" NONE);

  for (int i = 0; i < MAXCLIENTS; i++) {
    data[i] = (char *)calloc(1, 4096 + 10);
  }

  pthread_t *tid = (pthread_t *)calloc(INS, sizeof(pthread_t));

  for (int i = 0; i < INS; i++) {
    pthread_create(&tid[i], NULL, worker, (void *)taskQueue);
  }
  DBG(YELLOW "<D> : threads create successfully.\n" NONE);

  // 3. epoll相关设置
  if ((epollfd = epoll_create(1)) < 0) {
    perror("epoll_create");
    exit(1);
  }

  struct epoll_event events[MAXEVENTS], ev;

  ev.data.fd = server_listen;
  ev.events = EPOLLIN;
  clients[server_listen] = server_listen;
  // 设置epoll监听指定事件
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, server_listen, &ev) < 0) {
    perror("epoll_ctl");
    exit(1);
  }
  DBG(YELLOW "<D> : server_listen is added to epollfd successfully.\n" NONE);

  int sockfd;
  for (;;) {
    // -1代表永远等待, 要么超时, 被中断, 要么有>0的值返回,
    // 将返回的事件放在events中
    int nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
    if (nfds < 0) {
      perror("epoll_wait");
      exit(1);
    }
    for (int i = 0; i < nfds; i++) {
      int fd = events[i].data.fd;
      if (fd == server_listen && (events[i].events & EPOLLIN)) {
        // 如果是监听套接字, 执行以下逻辑

        if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
          perror("accept");
          exit(1);
        }
        clients[sockfd] = sockfd;
        ev.data.fd = sockfd;
        ev.events = EPOLLIN | EPOLLET;
        make_nonblock(sockfd);
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
          perror("epoll_ctl");
          exit(1);
        }
      } else {
        if (events[i].events & EPOLLIN) {
          // 套接字就绪, 有数据
          task_queue_push(taskQueue, (void *)&clients[fd]);
        } else {
          // 如果出现异常, 那么删除对该fd的监听, 并进行close关闭fd
          epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
          close(fd);
        }
      }
    }
  }
  return 0;
}