/**
 * @author: wuyiccc
 * @date: 2023-01-01 20:35
 */

#include "head.h"

const char *config = "./wechat.conf";

int main(int argc, char **argv) {
  int opt;
  int server_port = 0;
  int sex = 1;
  int sockfd;
  int mode = 0;

  char server_ip[20] = {0};
  char name[50] = {0};
  while ((opt = getopt(argc, argv, "p:h:s:n:m:")) != -1) {
    switch (opt) {
      case 'p':
        server_port = atoi(optarg);
        break;
      case 's':
        sex = atoi(optarg);
        break;
      case 'm':
        mode = atoi(optarg);
        break;
      case 'h':
        strcpy(server_ip, optarg);
        break;
      case 'n':
        strcpy(name, optarg);
        break;
      default:
        fprintf(stderr, "Usage : %s -p port -n name -s sex -h server_ip.\n",
                argv[0]);
        exit(1);
    }
  }

  if (access(config, R_OK)) {
    fprintf(stderr, RED "<Erro>" NONE " : config file error!\n");
    exit(1);
  }

  if (!server_port) {
    server_port = atoi(get_conf_value(config, "SERVER_PORT"));
  }
  if (sex < 0) {
    sex = atoi(get_conf_value(config, "SEX"));
  }
  if (!strlen(name)) {
    strcpy(name, get_conf_value(config, "NAME"));
  }
  if (!strlen(server_ip)) {
    strcpy(server_ip, get_conf_value(config, "SERVER_IP"));
  }
  DBG(YELLOW "<D>" NONE " : read config success.\n");
  DBG(BLUE "<D>" NONE " : server_ip = %s\n", server_ip);
  DBG(BLUE "<D>" NONE " : server_port = %d\n", server_port);
  if ((sockfd = socket_connect(server_ip, server_port)) < 0) {
    perror("socket_connect");
    exit(1);
  }
  DBG(YELLOW"<D>"NONE" : connection to server %s:%d --> <%d> successfully.\n", server_ip, server_port, sockfd);
  return 0;
}
