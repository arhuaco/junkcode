#include <caiterate.h>
#include <util.h>

#include <stdio.h>
#include <stdlib.h>


#define PROGRAM_NAME "checkmany"
#define DEBUG 1


CA *ca1 = 0;

inline float
check_one(unsigned int rule, int N, int size, int is_random)
{
  if (!ca1)
    ca1 = ca_create(size, rule);

  ca_rule_set(ca1, rule);

  if (is_random)
    ca_init_random(ca1);
  else
  {
    ca_init_single(ca1);
    ca_iterate (ca1,size / 2);
  }

  return ca_entropy_measure(ca1, N, 4);
}

char *FILE_NAME  = NULL;
int   BINARY     = 0;

void
check_many(int lambda_bits, int N, int size, int is_random, int how_many)
{
  unsigned int total_count = 0;
  unsigned int i;
  FILE *p;

  p = fopen(FILE_NAME, "w");

  ASSERT(p);

  for (i = 0; i < 0xffffffff && total_count < how_many; i++) /* for each EVEN CA rule */
  {
    int j;
    unsigned int bit = 1;
    int match_count = 0;

    if (i & 0x1) /* Only even rules allowed. */
      continue;

    for (j = 0; j < 32; ++j) /* lambda_bits bits in rule? */
    {
      if (i & bit)
        match_count++;

      if (match_count > lambda_bits)
        break;

      bit <<= 1;
    }

    if (match_count == lambda_bits) /* this rule is OK */
    {
      float ent;

      total_count++;

      ent = check_one(i /*rule*/, N, size, is_random);

      if (ent <= 1e-5) /* -0.0 sometimes. */
        ent = 0.0;

      if (BINARY)
      {
        int w;

        w = fwrite(&ent, sizeof(ent), 1, p);

        if (w != 1)
        {
          fprintf(stderr, "Could not write a float to the file!\n");
          exit(EXIT_FAILURE);
        }
      }
      else
        fprintf(p, "%g\n", ent);

      if (DEBUG && !(total_count % 1000))
      {
        printf ("%uk\n", total_count / 1000);
        fflush(p);
        fflush(stdout);
      }
    }
  }

  fclose(p);

}

int main(int argc, char *argv[])
{
  int lambda_bits, how_many, N, size, is_random;

  if (argc != 8)
  {
    fprintf(stderr, "usage: %s n-bits how_many n-steps size random? file_name binary?\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }

  lambda_bits= atoi(argv[1]);
  how_many   = atoi(argv[2]);
  N          = atoi(argv[3]);
  size       = atoi(argv[4]);
  is_random  = atoi(argv[5]);
  FILE_NAME  = argv[6];
  BINARY     = atoi(argv[7]);

  if (DEBUG)
  {
    printf("(lambda_bits %d) (how_many %d) (N %d) (size %d) (is_random %d)\n",
                    lambda_bits, how_many, N, size, is_random);
  }

  check_many(lambda_bits, N, size, is_random, how_many);

  return EXIT_SUCCESS;
}
