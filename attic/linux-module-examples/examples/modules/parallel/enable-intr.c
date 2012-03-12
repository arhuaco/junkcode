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

  outb(15, BASEPORT);
  outb(0x10, BASEPORT + 2); /* enable interrupts */

  printf("status: %d\n", inb(BASEPORT + 1));

  if (ioperm(BASEPORT, 3, 0))
  {
    perror("ioperm");
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
