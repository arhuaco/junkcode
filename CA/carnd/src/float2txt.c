#include <stdio.h>
#include <stdlib.h>

/* Convert a file with a sequence o floats to a text file.
 * Now that I remembet it, I think that octave knows how
 * to open such files */

#define PROGRAM_NAME "float2txt"

int
main(int argc, char *argv[])
{
  FILE *p;

  if (argc != 2)
  {
    fprintf(stderr, "Use: %s filename\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }
  
  p = fopen(argv[1], "r");

  if (!p)
  {
    fprintf(stderr, "%s : Couldn't open file '%s'\n", PROGRAM_NAME, argv[1]);
    exit(EXIT_FAILURE);
  }

  while(!feof(p))
  {
    float ent;

    if(fread(&ent, sizeof(ent), 1, p) != 1)
      break;

    printf("%g\n", ent);
  }

  fclose(p);

  return EXIT_SUCCESS;
}
