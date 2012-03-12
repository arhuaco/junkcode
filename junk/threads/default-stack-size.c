#include <stdio.h>
#include <pthread.h>

/* gcc default-stack-size.c -Wall -lpthread */

int
main(void)
{
  size_t x;
  pthread_attr_t tattr;
  pthread_attr_init(&tattr);
  if (!pthread_attr_getstacksize (&tattr, &x))
    fprintf(stderr, "%d\n", x);
  else
    perror("pthread_attr");
  return 0;
}
