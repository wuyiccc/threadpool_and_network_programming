/**
 * @author: wuyiccc
 * @date: 2023-01-01 17:20
 */
#ifndef _WECHAT_H
#define _WECHAT_H
#define MAXEVENTS 5
#define MAXUSERS 1024
struct wechat_user {
  char name[50];
  char passwd[20];
  int sex;
  int fd;
  int isOnline;
};

// 定义消息的比特掩码 1 2 4 8 16 32 ...
#define WECHAT_SIGUP 0x01
#define WECHAT_SIGIN 0x02
#define WECHAT_SIGOUT 0X04
#define WECHAT_ACK 0x08
#define WECHAT_NAK 0x10
#define WECHAT_SYS 0x20
#define WECHAT_WALL 0x40
#define WECHAT_MSG 0x80
#define WECHAT_FIN 0X100
#define WECHAT_HEART 0x200


struct wechat_msg {
  // 消息类型
  int type;
  int sex;
  char from[50];
  char to[50];
  char msg[1024];
};
void *sub_reactor(void *arg);
int add_to_reactor(int efd, int fd);
void *client_recv(void *arg);
#endif
