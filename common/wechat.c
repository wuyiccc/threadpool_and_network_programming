/**
 * @author: wuyiccc
 * @date: 2023-01-01 17:19
 */

#include "head.h"

void *sub_reactor(void *arg) {
  int subfd = *(int *)arg;
  while (1) {
    DBG(L_RED"<Sub Reactor>"NONE" : in sub reactor %d\n.", subfd);
    sleep(30);
  }
}