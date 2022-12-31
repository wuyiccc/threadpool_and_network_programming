/**
 * @author: wuyiccc
 * @date: 2022-12-28 20:47
 */

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

struct task_queue {
  int head, tail, size, total;
  void **data;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
};

void task_queue_init(struct task_queue *taskQueue, int size);
void task_queue_push(struct task_queue *taskQueue, void *data);
void *task_queue_pop(struct task_queue *taskQueue);

void *thread_run(void *arg);
void *worker(void *arg);
#endif