/**
 * @author: wuyiccc
 * @date: 2022-12-30 22:43
 */

#include "./common/head.h"

int main(int argc, char **argv) {
  int opt;
  int port = 777;
  int server_listen;
  int sockfd;
  while ((opt = getopt(argc, argv, "p:")) != 0) {
    switch (opt) {
      case 'p':
        port = atoi(optarg);
        break;
      default:
        fprintf(stderr, "Usage : %s -p port!\n", argv[0]);
        exit(1);
    }
  }
  DBG(YELLOW "<D> : Server will listen on port [%d].\n", port);
}