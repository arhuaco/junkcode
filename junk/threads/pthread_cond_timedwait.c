#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>

/* This is an example of the usage of pthread_cond_timedwait
 * gcc  -Wall pthread_cond_timedwait.c  -lpthread */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;

int  value = 0;

void *
signal_after_sometime(void *arg)
{

  if (usleep((int)arg * 1000))
  {
    fprintf(stderr, "error calling usleep\n");
    exit(1);
  }

  pthread_mutex_lock(&mutex);
  value = 1;
  fprintf(stderr, "signal!\n");
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);

  return NULL;
}

void *
wait_for_sometime(void *arg)
{
  struct timeval now;
  struct timespec timeout;
  long future_us;

  int r;

  pthread_mutex_lock(&mutex);

  gettimeofday(&now, NULL);
  /* microsecond precision */
  future_us = now.tv_usec + (int)arg * 1000;
  timeout.tv_nsec = (future_us % 1000000) * 1000;
  timeout.tv_sec = now.tv_sec + future_us / 1000000;

  r = 0;

  while (value == 0 && r != ETIMEDOUT && r != EINVAL && r != EPERM)
    r = pthread_cond_timedwait(&cond, &mutex, &timeout);

  if (r == ETIMEDOUT) {
    printf("timeout!\n");
    exit(0);
  }
  else if (r != 0)
  {
    printf("error in pthread_cond_timedwait call!\n");
    exit(1);
  }

  printf("OK :)\n");

  pthread_mutex_unlock(&mutex);

return NULL;
}

int
main(int argc, char *argv[])
{
  pthread_t t1, t2;

#if 0 /* this should not timeout */
  pthread_create( &t1, NULL, &signal_after_sometime, (void*)1900); /* milliseconds */
  pthread_create( &t2, NULL, &wait_for_sometime, (void*)2000); /* milliseconds */
#endif

  /* this should timeout after 0.9 seconds */
  pthread_create( &t1, NULL, &signal_after_sometime, (void*)1000); /* milliseconds */
  pthread_create( &t2, NULL, &wait_for_sometime, (void*)900); /* milliseconds */

  pthread_join( t1, NULL);
  pthread_join( t2, NULL);

  return 0;
}
