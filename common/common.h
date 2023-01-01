/**
 * @author: wuyiccc
 * @date: 2022-12-30 22:50
 */

#ifndef _COMMON_H
#define _COMMON_H

int socket_create(int port);
int socket_connect(const char *ip, int port);
int make_nonblock(int fd);
int make_block(int fd);
char *get_conf_value(const char *file, const char *key);
char ans[512];
#endif