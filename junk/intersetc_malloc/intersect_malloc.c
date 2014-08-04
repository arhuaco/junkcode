/*
  Intersect malloc and free. See this:
  http://stackoverflow.com/questions/6083337/overriding-malloc-using-the-ld-preload-mechanism

  LD_PRELOAD=./intersect_malloc.so ls
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <pthread.h>

static void* (*real_malloc)(size_t) = NULL;

static pthread_mutex_t memory_count_mutex = PTHREAD_MUTEX_INITIALIZER;

size_t memory_count = 0;

static void mtrace_init(void) {
  real_malloc = dlsym(RTLD_NEXT, "malloc");
  if (NULL == real_malloc) {
    fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    exit(1);
  }
}

void* malloc(size_t size) {
  if (real_malloc == NULL) {
    mtrace_init();
  }

  fprintf(stderr, "malloc(%zu) = ", size);

  pthread_mutex_lock(&memory_count_mutex);
  if (memory_count > 2L * 1024L * 1024L * 1024L) {
    pthread_mutex_lock(&memory_count_mutex);
    fprintf(stderr, " DENY!\n");
    return NULL;
  }
  pthread_mutex_unlock(&memory_count_mutex);


  void *p = real_malloc(size);
  if (p) {
    fprintf(stderr, " OK\n");
    pthread_mutex_lock(&memory_count_mutex);
    memory_count += size;
    pthread_mutex_unlock(&memory_count_mutex);
  } else {
    fprintf(stderr, " FAIL\n");
  }
  fprintf(stderr, "allocated: %zu\n", memory_count);
  return p;
}
