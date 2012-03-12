#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

/*
 * This is a small template for performing a task that cannot be queued.
 * It uses two condition variables and a mutex. This pattern is used in
 * test_timer.c.
 *
 * Thanks to the people in comp.programming.threads for their help.
 *
 * We tried to "polish" this example so that it can be useful for
 * others in the future.
 *
 */

#define misc_die(r) \
  do { \
  fprintf(stderr, "Dying. " __FILE__ ":%d Reason: %s\n", __LINE__, (r)); \
  exit(1); \
  } while(0)

static pthread_cond_t cond_work = PTHREAD_COND_INITIALIZER; /* a task to do */
static pthread_cond_t cond_done = PTHREAD_COND_INITIALIZER; /* the task has been done */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void *doexec (void *arg);

void
init (pthread_t *thr)
{
  pthread_attr_t thread_attr;

  if (pthread_attr_init(&thread_attr))
    misc_die("pthread_attr_init");

  if (pthread_create(thr, &thread_attr, doexec, NULL))
    misc_die("pthread_create");
}

int action = 0; /* 0 == count, 1 == exit : what doexec has to do */
int action_arg; /* argument of the action */
int done = 0;   /* to tell new_work that doexec is done */

int count = 0;  /* the simulated work : increment a counter */

void
new_work (int param)
{
  if (pthread_mutex_lock (&mutex))
    misc_die("pthread_mutex_lock");

  action_arg = param;

  if (pthread_cond_signal(&cond_work))
    misc_die("pthread_cond_signal");

  fprintf(stderr, "new_work> please add %d to count:%d\n", param, count);

  done = 0;
  while (!done)
    if (pthread_cond_wait (&cond_done, &mutex)) /* let's wait for doexec */
      misc_die("pthread_cond_wait");

  if (pthread_mutex_unlock (&mutex))
    misc_die("pthread_mutex_unlock");
}

static int ready_to_get_work = 0;

static void *
doexec (void *arg)
{
  if (pthread_mutex_lock (&mutex)) /* unlocked when waiting for condition */
    misc_die("pthread_mutex_lock");

  ready_to_get_work = 1; /* this will allow main(..) to continue */

  while (1 != action)
  {
    int ret;
    ret = pthread_cond_wait (&cond_work, &mutex); /* wait for work */

    if (0 == ret && 1 != action)
    {
      count += action_arg; /* do work */
      fprintf(stderr, " doexec> Ok, count:%d\n", count);
      done = 1;
      if (pthread_cond_signal(&cond_done)) /* tell do_work we're done */
        misc_die("pthread_cond_signal");
    }
    else if (ret)
      misc_die("pthread_cond_wait");
  }

  if (pthread_mutex_unlock (&mutex))
    misc_die("pthread_mutex_unlock");

  return NULL;
}

#define N 100

int
main (int argc, char *argv[])
{
  int i;
  pthread_t thread_id;

  /* Initialize the 'consumer' thread. we cannot queue tasks and we need
   * it to be a thread. */

  init(&thread_id);

  /* there are many ways to wait here. I prefer to use
   * condition variables but I wrote this code because
   * it is easier to understand what we need to do */

  while(!ready_to_get_work)
    usleep(10000); /* 1/100 seconds */

  /* do some work N times. The result of this sumation should
   * be N*(N+1))/2. */
  for (i = 1; i <= N; ++i)
    new_work (i);

  if (pthread_mutex_lock (&mutex))
    misc_die("pthread_mutex_lock");

  action = 1; /* tell doexec to finish */

  if (pthread_cond_signal(&cond_work))
    misc_die("pthread_cond_signal");

  if (pthread_mutex_unlock (&mutex))
    misc_die("pthread_mutex_unlock");

  fprintf(stderr, "\nFinal count:%d. We expect it to be %d\n",
          count, (N * (N + 1)) / 2);

  if (pthread_join(thread_id, NULL)) /* wait until working thread to returns */
    misc_die("pthread_join");

  return 0;
}
