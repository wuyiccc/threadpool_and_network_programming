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

  // 设置端口复用, 避免出现TIME_WAIT导致端口不可用的情况出现 
  int reuse = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int));

  // 将sockfd进行赋值
  if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    return -1;
  }

  if (listen(sockfd, 20) < 0) {
    return -1;
  }
  return sockfd;
}

int make_nonblock(int fd) {
  int flags;
  if ((flags = fcntl(fd, F_GETFL)) < 0) {
    return -1;
  }

  flags |= O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}

int make_block(int fd) {
  int flags;
  if ((flags = fcntl(fd, F_GETFL)) < 0) {
    return -1;
  }

  flags &= ~O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}

// 读取配置文件
char *get_conf_value(const char *file, const char *key) {
  FILE *fp;
  if ((fp = fopen(file, "r")) == NULL) {
    return NULL;
  }

  char *line = NULL;
  ssize_t len = 0;
  ssize_t nread = 0;
  char *sub = NULL;
  // 初始化ans
  bzero(ans, sizeof(ans));
  while ((nread = getline(&line, &len, fp)) != -1) {
    if ((sub = strstr(line, key)) == NULL) {
      continue;
    }
    // 如果key出现在行首, 且下一个字符是=
    if (sub == line && line[strlen(key)] == '=') {
      strcpy(ans, line + strlen(key) + 1);
      // 如果最后一个字符是回车, 那么将回车替换成终止符
      if (ans[strlen(ans) - 1] == '\n') {
        ans[strlen(ans) - 1] = '\0';
      }
    }
  }
  if (!strlen(ans)) {
    return NULL;
  }
  fclose(fp);
  free(line);
  return ans;
}