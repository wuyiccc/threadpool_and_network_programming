/**
 * @author: wuyiccc
 * @date: 2023-01-01 11:29
 */

#include "head.h"

// 基于当前目录(程序运行目录)的相对路径
const char *config = "./wechatd.conf";

int main(int argc, char **argv) {
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
    fprintf(stderr, RED"<Error>"NONE"config file error!\n");
    exit(1);
  }

  if (!port) {
    port = atoi(get_conf_value(config, "PORT"));
    DBG("port = %d!\n", port);
  }
  DBG(YELLOW"<D>"NONE" : server_listen is listening on port %d.\n", port);

  int server_listen;
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

  if ((subefd1 = epoll_create(1)) < 0) {
    perror("epoll_create");
    exit(1);
  }
  DBG(YELLOW"<D>"NONE" : Main reactor and two sub reactors created.\n");

  pthread_t tid1, tid2;
  pthread_create(&tid1, NULL, sub_reactor, (void *)&subefd1);
  pthread_create(&tid1, NULL, sub_reactor, (void *)&subefd2);
  DBG(YELLOW"<D>"NONE" : Two sub reactor thread created.\n");

  // 100 000 微妙
  usleep(100000);


  return 0;
}
