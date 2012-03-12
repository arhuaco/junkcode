#include <caiterate.h>
#include <util.h>

#include <stdlib.h>

#define PROGRAM_NAME "rng1"
#define DEBUG 1

int
main(int argc, char *argv[])
{
  /* TODO: call srand */
  CA *ca;
  char *filename = "out.rnd";
  FILE *p;
  int nbytes;
  long double min_ent;
  unsigned rule;

  nbytes = 1024*1024*10;

  if (argc != 2)
  {
    fprintf(stderr, "usage: %s rule\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }


  sscanf(argv[1], "%u", &rule);

  if (DEBUG)
  {
    printf("rule %u\n", rule);
  }
#if 0
  ca = ca_create(512, 0xa6965966u); /*good*/
#endif


  ca = ca_create(512,  rule);

  if (! (p = fopen(filename, "wb")))
  {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  ca_init_random(ca);
  min_ent = ca_entropy_measure(ca, 1024, 4);

  printf("\nminent = %Lg\n", min_ent);

  ca_init_random(ca);

  while (nbytes >= 0)
  {
    ca_iterate(ca, 1);
    nbytes = ca_to_file(ca, nbytes, p);
  }

  fclose(p);
  ca_destroy(ca);

  return EXIT_SUCCESS;
}
