#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/*
 *
 *
 * This code has BUGS. I won't modify it because I posted it in a public
 * group.
 *
 * The fixed code is : cond-signal-test-ok.c
 *
 * Thanks.
 */

#define misc_die(r) \
  do { \
  fprintf(stderr, "Dying... %s:%d Reason: %s\n", __FILE__, __LINE__, (r)); \
  exit(1); \
  } while(0)

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;

static void *doexec (void *arg);

void
init (void)
{
  pthread_t thr;
  pthread_attr_t thread_attr;

  if (pthread_attr_init(&thread_attr))
    misc_die("pthread_attr_init");

  if (pthread_create(&thr, &thread_attr, doexec, NULL))
    misc_die("pthread_create\n");
}

int action = 0; /* 0 == count, 1 == exit */
int count = -1;
int ready = 0;

void
dosignal (void)
{
  pthread_mutex_lock (&cond_mutex);
  pthread_cond_signal(&cond);
  pthread_mutex_unlock (&cond_mutex);
  sched_yield();
}

static void *
doexec (void *arg)
{
  pthread_mutex_lock (&cond_mutex); /* unlocked when waiting for condition */

  ready = 1;

  while (action != 1)
  {
    count++;
    pthread_cond_wait (&cond, &cond_mutex);
  }
  pthread_mutex_unlock (&cond_mutex);
  pthread_detach(pthread_self());

  return NULL;
}

#include <sched.h>

int
main (int argc, char *argv[])
{
  int i;

#if 0
  struct sched_param param;
  param.sched_priority = 20;
  if(sched_setscheduler(0, SCHED_FIFO, &param)==-1)
  {
    perror("sched_setscheduler");
    return -1;
  }
#endif

  init();

  while (!ready)
    usleep(10000);

  for (i = 0; i < 1000; ++i)
    dosignal ();

  pthread_mutex_lock (&cond_mutex);
  action = 1;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock (&cond_mutex);

  usleep(10000);
  fprintf(stderr, "%d\n", count);

  return 0;
};
