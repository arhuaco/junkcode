#include <stdio.h>
#include <stdlib.h>


/* Align to 32 bytes boundaries */
char *
align_pointer (char *p)
{
  int x = (int)p;
  x =  x & ~0x1F;
  return (char*)x;
}

int
main (int argc, char *argv[])
{
  char *p, *q;
  int size = 100;

  p = malloc(size + 32);
  q = align_pointer(p);

  printf("raw:%d aligned:%d\n", p, q);
  printf("raw:%p aligned:%p\n", p, q);

  /* Use q, but remember to free p */

  return 0;
}
