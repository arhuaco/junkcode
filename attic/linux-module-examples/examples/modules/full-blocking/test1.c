#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>

/*probar con el siguiente programa de python 

> f = open("/dev/fullex0", "w")
> f.write("1")
> f.flush()
> f.write("1")
> f.flush()
> f.close()

*/


int main(int argc, char *argv[])
{
  FILE *p;

  p = fopen("/dev/fullex0", "r");

  if (!p)
  {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  /* Leamos de a 1 */
  while (!feof(p))
  {
    char c;
    int r;
    r = fread(&c, 1, 1, p);

    if (0 == r && ferror(p))
    {
      perror("fwrite");
      exit(EXIT_FAILURE);
    }
    else
    {
      printf("Leído (caracter %d ('%c'))\n", c, c);
      fflush(stdout);
    }
  }

  fclose(p);

  return EXIT_SUCCESS;
}
