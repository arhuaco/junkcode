/*
  Intersect malloc and free. See this:
  http://stackoverflow.com/questions/6083337/overriding-malloc-using-the-ld-preload-mechanism

  LD_PRELOAD=./intersect_malloc.so ls

  This spinlock thing is not tested.

 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <pthread.h>

static void* (*real_malloc)(size_t) = NULL;

static pthread_mutex_t memory_count_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_spinlock_t spin_lock;

volatile size_t memory_count = 0;

static void intersect_init(void) {
  if(pthread_spin_init(&spin_lock, PTHREAD_PROCESS_SHARED)) {
    fprintf(stderr, "Error in `pthread_spin_init`\n");
    exit(1);
  }
  real_malloc = dlsym(RTLD_NEXT, "malloc");
  if (NULL == real_malloc) {
    fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    exit(1);
  }
}

void* malloc(size_t size) {
  if (real_malloc == NULL) {
    pthread_mutex_lock(&memory_count_mutex);
    /* I think this double check is an anti-pattern. Let's do it here. Nobody is watching. */
    if (real_malloc == NULL) {
      intersect_init();
    }
    pthread_mutex_unlock(&memory_count_mutex);
  }

  fprintf(stderr, "malloc(%zu) = ", size);

  while (pthread_spin_lock(&spin_lock));
  if (memory_count > 2L * 1024L * 1024L * 1024L) {
    while (pthread_spin_unlock(&spin_lock));
    fprintf(stderr, " DENY!\n");
    return NULL;
  }
  while (pthread_spin_unlock(&spin_lock));


  void *p = real_malloc(size);
  if (p) {
    fprintf(stderr, " OK\n");
    while (pthread_spin_lock(&spin_lock));
    memory_count += size;
    while (pthread_spin_unlock(&spin_lock));
  } else {
    fprintf(stderr, " FAIL\n");
  }
  fprintf(stderr, "allocated: %zu\n", memory_count);
  return p;
}
