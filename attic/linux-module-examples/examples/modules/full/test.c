#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "fullex.h"

int main(int argc, char *argv[])
{
  FILE *p;
  int r;
  int prm;

  p = fopen("/dev/fullex0", "w");

  if (!p)
  {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  r = ioctl(fileno(p), FULLEX_RESET, NULL);

  if (r)
    perror("ioctl");

  printf("FULLEX_RESET returned %d\n", r);

  r = ioctl(fileno(p), FULLEX_GETSIZE, &prm);

  if (r)
    perror("ioctl");
  
  printf("FULLEX_GETSIZE returned %d, size is %d\n", r, prm);

  prm = 1999;

  r = ioctl(fileno(p), FULLEX_SETSIZE, &prm);

  if (r)
    perror("ioctl");
  
  printf("FULLEX_SETSIZE returned %d\n", r);

  r = fwrite ("1", 1, 1, p);

  if (ferror(p))
  {
    perror("fwrite");
    exit(EXIT_FAILURE);
  }

  fclose(p);

  return EXIT_SUCCESS;
}
