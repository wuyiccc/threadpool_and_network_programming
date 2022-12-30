/**
 * @author: wuyiccc
 * @date: 2022-12-28 23:41
 */

#include "./common/head.h"

int main() {

  int flag = 0;
  // 设置标准输入为5s
  fd_set rfds;
  FD_ZERO(&rfds);
  // 只关注0号文件
  FD_SET(0, &rfds);

  // 文件描述符数量1
  struct timeval tv;
  // 设置5s整
  tv.tv_sec = 5;
  tv.tv_usec = 0;
  // 最多阻塞等待5s
  int n = select(1, &rfds, NULL, NULL, &tv);
  if (n > 0) {
    printf("input found\n");
    scanf("%d", &flag);
  } else {
    printf(RED"Time out!\n"NONE);
  }
  printf("flag = %d\n", flag);
  return 0;
}