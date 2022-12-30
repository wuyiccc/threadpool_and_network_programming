/**
 * @author: wuyiccc
 * @date: 2022-12-30 22:50
 */

#include "head.h"

int socket_create(int port) {
  int sockfd;

  // 存储绑定端口需要的信息
  struct sockaddr_in server;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  }
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);  // 监听任意的ip

  // 将sockfd进行赋值
  if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    return -1;
  }

  if (listen(sockfd, 20) < 0) {
    return -1;
  }
  return sockfd;
}