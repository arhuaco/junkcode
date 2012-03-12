#include <stdio.h>
#include <stdlib.h>

/* Switch the endiannes of a file with a sequence of floats */

#define PROGRAM_NAME "floatswap"

// http://www.gamedev.net/reference/articles/article2091.asp
inline float FloatSwap(float f)
{
  union
  {
    float f;
    unsigned char b[4];
  } dat1, dat2;

  dat1.f = f;
  dat2.b[0] = dat1.b[3];
  dat2.b[1] = dat1.b[2];
  dat2.b[2] = dat1.b[1];
  dat2.b[3] = dat1.b[0];
  return dat2.f;
}

int
main(int argc, char *argv[])
{
  FILE *p;
  FILE *q;

  if (argc != 3)
  {
    fprintf(stderr, "Use: %s src dst\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }
  
  p = fopen(argv[1], "r");

  if (!p)
  {
    fprintf(stderr, "%s : Couldn't read file '%s'\n", PROGRAM_NAME, argv[1]);
    exit(EXIT_FAILURE);
  }

  q = fopen(argv[2], "w");

  if (!q)
  {
    fprintf(stderr, "%s : Couldn't write file '%s'\n", PROGRAM_NAME, argv[2]);
    exit(EXIT_FAILURE);
  }

  while(!feof(p))
  {
    float ent;

    if(fread(&ent, sizeof(ent), 1, p) != 1)
      break;

    //printf("%g (era %g)\n", FloatSwap(ent), ent);
    ent = FloatSwap(ent);

    if (fwrite(&ent, sizeof(ent), 1, q) != 1)
      fprintf(stderr, "Hey! Didn't write the float!\n");
  }

  fclose(p);
  fclose(q);

  return EXIT_SUCCESS;
}
