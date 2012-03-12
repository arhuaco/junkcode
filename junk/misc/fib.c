#include <stdio.h>
#include <stdlib.h>

unsigned int
fib_n(unsigned int N)
{
  int i;
  unsigned int actual = 0;
  unsigned int next = 1;

  for (i = 0; i < N; ++i)
  {
    unsigned int tmp;
    tmp = actual;
    actual = next;
    next+= tmp;
  }

  return actual;
}

int
main (int argc, char *argv[])
{
  unsigned int N;

  if (argc != 2)
  {
    fprintf(stderr, "Use: %s N\n", argv[0]);
    return 1;
  }

  N = atoi(argv[1]);

  printf("Fibonacci # %d is %u\n", N, fib_n(N));
}
