
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static pthread_mutex_t th_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t th_cond = PTHREAD_COND_INITIALIZER;
static int n = 0;

void *fun(void *p) {
  int i;
  int *val = p;
  printf("test\n");
  for (i = 0; i < 10; i++) {
    pthread_cond_wait(&th_cond, &th_lock);
    printf("thread %d: n = %d\n", *val, n);
  }
  return NULL;
}

int main(void) {
  pthread_t pid1, pid2;
  int i = 0;

  pthread_create(&pid1, NULL, fun, (void *)1);
  pthread_create(&pid2, NULL, fun, (void *)2);
  for (; i < 30; i++) {
    sleep(1);
    pthread_cond_signal(&th_cond);
  }

  return 0;
}