#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

/*
 * This is a timer that we need to port code from OSs that have
 * a timer that can be scheduled and cancelled. We have a simple
 * pool of threads. When the specified time has elapsed, the timer
 * will wake up a thread that will execute a callback function.
 *
 * Nelson Castillo < nelson at emqbit.com >
 * Andres Calderon < andres.calderon at emqbit.com >
 *
 *
 * Tue Feb 19 2008 : Fixed a synchronization issue.
 *
 * TODO:
 *
 * - cleanup function : terminates the threads in the pool and timer_exec.
 * - use wrappers to check the return values of ALL the functions.
 * - in the original code we need to port, we will only have about 3 timers
 *   used at once. We do know that we can do a better use of the threads in
 *   the pool, but the simpler (and less optimal) 1-1 approach will work for
 *   us now. Early optimization is the root of all evil.
 *
 */

#define NJOBS 5

#define OK 1
#define ERR -1 /* 0 is a valid return value */

typedef long long timestamp_t;

typedef void (*timer_callback_function)(unsigned int id, void * arg);

typedef struct _task_callback
{
  timestamp_t timestamp;       /* absolute time for the execution */
  timestamp_t delay;           /* needed to restart the timer */
  timer_callback_function f;   /* function to be executed */

  void* arg;

  int index;
  struct _task_callback *next;
  struct _task_callback *prev;

  pthread_t tid;

  pthread_mutex_t condition_mutex;
  pthread_cond_t condition;

  int active;
} task_callback;

#define misc_die(r) \
  do { \
  fprintf(stderr, "Dying... %s:%d Reason: %s\n", __FILE__, __LINE__, (r)); \
  exit(1); \
  } while(0)

enum timer_action_t {INIT_TIMER = 1, NEW_TIMER, REMOVE_TIMER, ACTION_NONE} timer_action_t;
static enum timer_action_t timer_action = ACTION_NONE;
static int timer_action_arg;

static pthread_cond_t timer_cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cancel_cond = PTHREAD_COND_INITIALIZER;
static pthread_cond_t add_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t timer_mutex = PTHREAD_MUTEX_INITIALIZER;
static void* timer_exec (void *arg);

static task_callback* tasks[NJOBS]; /* list of available threads */
static int avail_queue [NJOBS]; /* index of available threads */
static int n_avail = NJOBS;
static task_callback *tcl = NULL; /* task callback list */

static timestamp_t
get_timestamp ()
{
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec / 1000 + (timestamp_t)now.tv_sec * 1000;
}


/*
 * This function is part of the thread pool. It will execute the callback
 * function when the condition changes (when tc->active).
 */

static void *
timer_task (void *arg)
{
  task_callback *tc = (task_callback *) arg;

  while (1)
    {
      pthread_mutex_lock (&tc->condition_mutex);

      while (!tc->active)
        pthread_cond_wait (&tc->condition, &tc->condition_mutex);

      tc->active = 0; /* active means not in queue for execution */
      pthread_mutex_unlock (&tc->condition_mutex);

      (*tc->f) (tc->index, tc->arg);
    }

  return NULL;
}

void
timer_init (void)
{
  int i;
  pthread_t timer_thread;

  pthread_attr_t thread_attr;

  if (pthread_attr_init(&thread_attr))
    misc_die("pthread_attr_init");

  for (i = 0; i < NJOBS; i++)
    {
      task_callback *n;

      n = (task_callback *)malloc(sizeof(task_callback));
      if (!n)
        misc_die("malloc");
      memset(n, 0, sizeof(task_callback));

      n->index = i; /* job identifier */
      n->prev = n->next = NULL;
      n->active = 0;

      tasks[i] = n;
      avail_queue[i] = i;

      if (pthread_cond_init(&n->condition, NULL))
        misc_die("pthread_cond_init");

      if (pthread_create(&n->tid, &thread_attr, &timer_task, (void *) n))
        misc_die("pthread_create");
    }

  tcl = (task_callback *)malloc(sizeof(task_callback)); /* head of the list of current timers */
  if (!tcl)
    misc_die("malloc");

  memset(tcl, 0, sizeof(task_callback));
  tcl->next = NULL; /* not needed */
  tcl->index = -1;

  pthread_mutex_lock(&timer_mutex);
  timer_action = INIT_TIMER;

  if (pthread_create(&timer_thread, &thread_attr, timer_exec, NULL))
    misc_die("pthread_create");

  while (timer_action != ACTION_NONE)
    pthread_cond_wait(&cancel_cond, &timer_mutex);
  pthread_mutex_unlock(&timer_mutex);
}

int
timer_alloc (void)
{
  int ret = ERR;

  pthread_mutex_lock (&timer_mutex);

  if (n_avail > 0)
  {
    ret = avail_queue[--n_avail];
    tasks[ret]->active = 0;
  }

  pthread_mutex_unlock (&timer_mutex);

  return ret;
}

int
timer_free (int job)
{
  if (job < 0 || job >= NJOBS || tasks[job]->prev != NULL)
    misc_die("timer_free");

  pthread_mutex_lock (&timer_mutex);

  avail_queue[n_avail++] = job;

  pthread_mutex_unlock (&timer_mutex);

  return 1;
}

int did_add; /* variable to coordinate timer_schedule with timer_exec */

int
timer_schedule (int job, unsigned int delay,  void* cb, void* user_data)
{
  task_callback *last = tcl;
  task_callback *cur = tcl->next;
  task_callback *new_t;

  if (job < 0 || job >= NJOBS || tasks[job]->prev != NULL)
    misc_die("timer_schedule");

  new_t = tasks[job];

  delay *= 10;  /* 1 tick == 10 ms */

  new_t->delay = delay;
  new_t->timestamp = get_timestamp() + (timestamp_t)delay;
  new_t->f = (timer_callback_function)cb;
  new_t->arg = user_data;

  pthread_mutex_lock (&timer_mutex);

  while (cur && new_t->timestamp > cur->timestamp)
    {
      last = cur;
      cur = cur->next;
    }

  new_t->next = last->next;
  new_t->prev = last;
  last->next = new_t;
  if (new_t->next)
    new_t->next->prev = new_t;

  timer_action = NEW_TIMER;
  pthread_cond_signal(&timer_cond);

  did_add = 0;
  while (!did_add)
    pthread_cond_wait(&add_cond, &timer_mutex);

  pthread_mutex_unlock (&timer_mutex);

  return OK;
}

int did_cancel; /* variable to coordinate timer_cancel with timer_exec */

int
timer_cancel (int job)
{
  int ret = ERR;

  if (job < 0 || job >= NJOBS)
    misc_die("timer_schedule");

  pthread_mutex_lock (&timer_mutex);

  if (tasks[job]->prev != NULL)
    {
      timer_action_arg = job;
      timer_action = REMOVE_TIMER;

      pthread_cond_signal(&timer_cond);

      did_cancel = 0;
      while (!did_cancel)
        pthread_cond_wait(&cancel_cond, &timer_mutex);

      ret = OK;
    }

  pthread_mutex_unlock (&timer_mutex);

  return ret;
}

int
timer_reset(int id)
{
  task_callback *t;

  if (id < 0 || id >= NJOBS)
    misc_die("timer_reset, invalid job id");

  t = tasks[id];

   /*
    * This will interrupt the waiting thread twice. Let's not optimize yet.
    */

  if (timer_cancel(id) != OK)
    return ERR;

  if (timer_schedule(id, t->delay / 10, t->f, t->arg) != OK)
    return ERR;

  return OK;
}

static void
unlink_task_node(task_callback *d)
{
   if (d->prev == NULL)
     misc_die ("unlink_task_node");

   d->prev->next = d->next;
   if (d->next)
     d->next->prev = d->prev;
   d->next = d->prev = NULL;
}


static void *
timer_exec(void *arg)
{
  pthread_mutex_lock (&timer_mutex); /* unlocked when waiting for condition */

  timer_exec_next:

  switch (timer_action)
  {
    case ACTION_NONE: break;
    case NEW_TIMER:
      did_add = 1;
      pthread_cond_signal(&add_cond);
      break;
    case REMOVE_TIMER:
      unlink_task_node(tasks[timer_action_arg]);
      did_cancel = 1;
      pthread_cond_signal(&cancel_cond);
      break;
    case INIT_TIMER:
      timer_action = ACTION_NONE;
      pthread_cond_signal(&cancel_cond);
      break;
    default: misc_die("timer_exec");
  }

  timer_action = ACTION_NONE;

  if (tcl->next == NULL)
    pthread_cond_wait (&timer_cond, &timer_mutex);
  else
    {
      task_callback *wait_task = tcl->next;
      timestamp_t when = wait_task->timestamp;
      int do_exec = 0;

      if (when - get_timestamp() > 0)
        {
          struct timespec abstimeout;
          int r = 0;

          abstimeout.tv_sec = when / (timestamp_t)1000L;
          abstimeout.tv_nsec = (when - ((timestamp_t)abstimeout.tv_sec * (timestamp_t)1000L))
                               * (timestamp_t)1000000L;

          while (timer_action == ACTION_NONE &&
                 (r != ETIMEDOUT && r != EINVAL && r != EPERM))
            r = pthread_cond_timedwait(&timer_cond, &timer_mutex, &abstimeout);

          if (r == ETIMEDOUT)
            do_exec = 1;
          else if (timer_action == ACTION_NONE)
              misc_die("pthread_cond_timedwait");
        }
      else
        do_exec = 1;

      if (do_exec)
        {
          unlink_task_node(wait_task);

          pthread_mutex_lock (&wait_task->condition_mutex);
          wait_task->active = 1;
          pthread_cond_signal (&wait_task->condition);
          pthread_mutex_unlock (&wait_task->condition_mutex);
        }
    }

  goto timer_exec_next;
}

struct timeval tfirst;

void
test_f (unsigned int id, void * arg)
{
  struct timeval tnow;
  timestamp_t elapsed;
  gettimeofday(&tnow, NULL);
  elapsed = (tnow.tv_sec - tfirst.tv_sec) * 1000 +
            (tnow.tv_usec - tfirst.tv_usec) / 1000;
  fprintf(stderr, " * callback (id:%u) called  after %Lu ms\n", id, elapsed);
}

#define tick_1s 100.0

#include <stdlib.h>

int
main (int argc, char *argv[])
{
  int i;

  timer_init();
  gettimeofday(&tfirst, NULL);

  for (i = 0; i < NJOBS; ++i)
  {
    int id;
    if ((id = timer_alloc ()) == ERR)
      {
        fprintf(stderr, "couldn't add job # %d\n", i);
        exit(1);
      }
    fprintf(stderr, "allocated job # %d:%d\n", i,  id);
  }

  for (i = 0; i < NJOBS; ++i)
  {
    int delay = tick_1s * 0.1 * (float)(1 + i);
    fprintf(stderr, "task %d waiting for %d ticks\n", i, delay);
    timer_schedule (i, delay, (void*)test_f, NULL);
  }

  usleep(2500000);
  fprintf(stderr, "\nYou should have seen %d calls\n", NJOBS);

  for (i = 0; i < NJOBS; ++i)
    timer_schedule (i, tick_1s, (void*)test_f, NULL);
  for (i = 0; i < NJOBS; ++i)
  {
    if (timer_cancel(i) == ERR)
      fprintf(stderr, "couldn't remove timer %d\n", i);
  }

  fprintf(stderr, "\n\nNo function call should be made now ... ");
  usleep(1500000);
  fprintf(stderr, "OK? scheduling new timers...\n\n");

  gettimeofday(&tfirst, NULL);

  for (i = 0; i < NJOBS; ++i)
  {
    int delay = tick_1s * 0.1 * (float)(1 + i);
    fprintf(stderr, "task %d waiting for %d ticks\n", i, delay);
    timer_schedule (i, delay, (void*)test_f, NULL);
  }

  /* TODO: make cleanup code and wait for the thread */
  usleep(2500000);

  fprintf(stderr, "\nYou should have seen %d calls\n", NJOBS);

  return 0;
}
