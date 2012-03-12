#include <stdio.h>
#include <stdlib.h>

void
imprimir_mensaje(char *m)
{
  printf("%s", m);
}

int
main(int argc, char* argv[])
{
  int i;

  for(i = 0; i < argc; ++i)
  {
    imprimir_mensaje(argv[i]);
    imprimir_mensaje("\n");
  }

  return EXIT_SUCCESS;
}
