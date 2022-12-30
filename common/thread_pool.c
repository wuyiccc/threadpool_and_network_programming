/**
 * @author: wuyiccc
 * @date: 2022-12-28 20:48
 */

#include "head.h"

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
  DBG(GREEN "<push> :" RED "data" NONE "is pushed!\n");
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