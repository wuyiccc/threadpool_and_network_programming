/**
 * @author: wuyiccc
 * @date: 2022-12-30 22:50
 */

#ifndef _COMMON_H
#define _COMMON_H

int socket_create(int port);
int make_nonblock(int fd);
int make_block(int fd);
#endif