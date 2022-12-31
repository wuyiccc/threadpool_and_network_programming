/**
 * @author: wuyiccc
 * @date: 2022-12-28 20:48
 */

#include "head.h"

extern int epollfd;
extern char *data[2000];
extern pthread_mutex_t mutex[2000];

void task_queue_init(struct task_queue *taskQueue, int size) {
  taskQueue->size = size;
  taskQueue->total = taskQueue->head = taskQueue->tail = 0;
  taskQueue->data = calloc(size, sizeof(void *));
  pthread_mutex_init(&taskQueue->mutex, NULL);
  pthread_cond_init(&taskQueue->cond, NULL);
  return;
}

void task_queue_push(struct task_queue *taskQueue, void *data) {
  pthread_mutex_lock(&taskQueue->mutex);
  if (taskQueue->total == taskQueue->size) {
    DBG(YELLOW "<push> : taskQueue is full\n" NONE);
    pthread_mutex_unlock(&taskQueue->mutex);
    return;
  }

  taskQueue->data[taskQueue->tail] = data;
  DBG(GREEN "<push> :" RED "data " NONE "is pushed!\n");
  taskQueue->tail++;
  taskQueue->total++;
  if (taskQueue->tail == taskQueue->size) {
    DBG(YELLOW "<push> taskQueue reach tail!\n" NONE);
    taskQueue->tail = 0;
  }
  pthread_cond_signal(&taskQueue->cond);
  pthread_mutex_unlock(&taskQueue->mutex);
  return;
}

void *task_queue_pop(struct task_queue *taskQueue) {
  pthread_mutex_lock(&taskQueue->mutex);
  while (taskQueue->total == 0) {
    pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);
  }
  void *data = taskQueue->data[taskQueue->head];
  DBG(GREEN "<pop> :" BLUE " data " NONE "is poped!\n");
  taskQueue->total--;
  taskQueue->head++;
  if (taskQueue->head == taskQueue->size) {
    DBG(YELLOW "<pop> taskQueue reach tail!\n" NONE);
    taskQueue->head = 0;
  }
  pthread_mutex_unlock(&taskQueue->mutex);
  return data;
}

void *thread_run(void *arg) {
  pthread_detach(pthread_self());
  struct task_queue *taskQueue = (struct task_queue *)arg;
  while (1) {
    void *data = task_queue_pop(taskQueue);
    printf("%s", (char *)data);
  }
}

void do_work(int fd) {
  char buff[4096] = {0};
  DBG(BLUE "<R> data is ready on %d.\n" NONE, fd);
  int ind = strlen(data[fd]);
  int rsize = 0;
  if ((rsize = recv(fd, buff, sizeof(buff), 0)) < 0) {
    // 接收到的大小小于0, 代表出现异常, 关闭连接
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
    DBG(RED "<C> : %d is closed.\n" NONE, fd);
    close(fd);
    return;
  }
  pthread_mutex_lock(&mutex[fd]);
  for (int i = 0; i < rsize; i++) {
    if (buff[i] >= 'A' && buff[i] <= 'Z') {
      // 大写转小写
      data[fd][ind++] = buff[i] + 32;
    } else if (buff[i] >= 'a' && buff[i] <= 'z') {
      data[fd][ind++] = buff[i] - 32;
    } else {
      data[fd][ind++] = buff[i];
      if (buff[i] == '\n') {
        DBG(GREEN"%s"NONE, data[fd]);
        DBG(GREEN "<END> : \\n received!\n" NONE);
        send(fd, data[fd], ind, 0);
      }
    }
  }
  pthread_mutex_unlock(&mutex[fd]);
}

void *worker(void *arg) {
  pthread_detach(pthread_self());
  struct task_queue *taskQueue = (struct task_queue *)arg;
  while (1) {
    int *fd = (int *)task_queue_pop(taskQueue);
    do_work(*fd);
  }
}