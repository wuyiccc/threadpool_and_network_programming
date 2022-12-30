/**
 * @author: wuyiccc
 * @date: 2022-12-28 22:02
 */
#include "./common/head.h"

#define INS 4
#define SIZE 1000

int main() {
  FILE *fp;
  pthread_t tid[INS];
  char buff[SIZE][1024];
  struct task_queue *taskQueue =
      (struct task_queue *)malloc(sizeof(struct task_queue));
  task_queue_init(taskQueue, SIZE);

  for (int i = 0; i < INS; i++) {
    pthread_create(&tid[i], NULL, thread_run, (void *)taskQueue);
  }

  while (1) {
    if ((fp = fopen("./thread_pool_main.c", "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    int ind = 0;
    while (fgets(buff[ind], 1024, fp) != NULL) {
      task_queue_push(taskQueue, buff[ind]);
      if (++ind == SIZE) {
        ind = 0;
      }
      usleep(10000);
    }
    fclose(fp);
  }
  return 0;
}