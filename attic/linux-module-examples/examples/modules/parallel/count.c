#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h> 

#define BASEPORT 0x378

int main()
{
  int i;

  if (ioperm(BASEPORT, 3, 1))
  {
    perror("ioperm");
    exit(EXIT_FAILURE);
  }

  for (i = 0;;i++)
  {
    outb(i % 16, BASEPORT);
    printf("%d\n", i);
    usleep(1000000);
  }
  
  printf("status: %d\n", inb(BASEPORT + 1));

  if (ioperm(BASEPORT, 3, 0))
  {
    perror("ioperm");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
