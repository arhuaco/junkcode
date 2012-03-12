/* Testing merge sort with threads - just a test */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <pthread.h>
#include <cassert>
#include <algorithm>


using namespace std;

struct Work{
  vector <int> v;
  pthread_mutex_t *lock;
  int *nthreads;
};

void *merge_sort_thread(void *ptr) {
  Work *w = (Work *)ptr;
  if (w->v.size() <= 1)
    return NULL;
  if (w->v.size() <= 32) { // Auxiliar bubble sort.
    for (unsigned i = 0; i < w->v.size(); i++)
      for (unsigned j = i + 1 ; j < w->v.size(); j++)
        if (w->v[i] > w->v[j])
          swap(w->v[i], w->v[j]);
     return NULL;
  }

  Work left;
  Work right;
  left.v.reserve(w->v.size() / 2);
  right.v.reserve(w->v.size() / 2 + 1);
  left.lock = right.lock = w->lock;
  left.nthreads = right.nthreads = w->nthreads;

  unsigned i = 0;
  while (i < w->v.size() / 2)
    left.v.push_back(w->v[i++]);
  while (i < w->v.size())
    right.v.push_back(w->v[i++]);

  // FIXME: Compute a better threshold depending on array size.
  if (w->v.size() > 10000 && *w->nthreads > 1) {
    bool create = false;
    pthread_mutex_lock(w->lock);
    if (*w->nthreads > 1) {
      create = true;
      *w->nthreads -= 2;
    }
    pthread_mutex_unlock(w->lock);
    if (create) {
      pthread_t t1, t2;
      int r;
      cerr << "yes" << endl;
      r = pthread_create(&t1, NULL, merge_sort_thread, &left);
      r |= pthread_create(&t2, NULL, merge_sort_thread, &right);
      assert(!r); // TODO: better error handling.

      pthread_join(t1, NULL);
      pthread_join(t2, NULL);

      pthread_mutex_lock(w->lock);
      *w->nthreads += 2;
      pthread_mutex_unlock(w->lock);
    } else {
      merge_sort_thread(&left);
      merge_sort_thread(&right);
    }
  } else {
    merge_sort_thread(&left);
    merge_sort_thread(&right);
  }

  i = 0;
  unsigned j = 0;
  unsigned k = 0;

  while (i < left.v.size() && j < right.v.size()) {
    if (left.v[i] < right.v[j])
      w->v[k++] = left.v[i++];
    else
      w->v[k++] = right.v[j++];
  }

  while (i < left.v.size())
    w->v[k++] = left.v[i++];
  while (j < right.v.size())
    w->v[k++] = right.v[j++];
}

void test(unsigned size, int threads) {
  Work w;
  int nthreads = threads;
  for (unsigned i = 0; i < size; ++i)
    w.v.push_back(10 * ((double)random()/(double)RAND_MAX));

  pthread_mutex_t merge_lock = PTHREAD_MUTEX_INITIALIZER;
  w.lock = &merge_lock;
  w.nthreads= &nthreads;
  merge_sort_thread(&w);
}

//int
//main(int argc, char *argv[]) {
//  test(100000000, 2);
//}
