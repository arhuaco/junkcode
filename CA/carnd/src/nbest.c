#include <util.h>

#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_NAME "n-best"
#define DEBUG 1
#define MINIMUM_ENTROPY 3.8

typedef struct 
{
  unsigned int rule;  /* rule */
  float ent;          /* entropy */
} NODE;

static int
node_comparator (const NODE *a, const NODE *b)
{
  if(a->ent > b->ent) /* the comparison is inverted */
    return -1;

  if(a->ent < b->ent)
    return 1;

  return 0;
}


void
get_n_best(int lambda_bits, int n_best, FILE **p)
{
  unsigned int total_count = 0;
  unsigned int i;
  int n_floats;

  NODE *all;
  int n_considered = 0;


  fseek(p[0], 0, SEEK_END);
  n_floats = ftell(p[0]) / 4;
  rewind(p[0]);

  /* We won't use all that space
   * If memory is important we should use realloc */
  MALLOC(all, n_floats * sizeof(NODE));

  ASSERT(n_floats % 4 == 0);

  fprintf(stderr, "Floats in file: %d\n", n_floats);

  for (i = 0; i < 0xffffffff && total_count < n_floats; i++)
  /* for each EVEN CA rule */
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
      float ent, min_ent = MINIMUM_ENTROPY + 1000; 
      unsigned int nfile = 0;

      while (p[nfile])
      {
        if (1 != fread(&ent, sizeof(ent), 1, p[nfile]))
        {
          fprintf(stderr, "Could not read a float from the file!\n");
          exit(EXIT_FAILURE);
        }

        if (ent < min_ent)
          min_ent = ent;

        nfile++;
      }

      if (min_ent > MINIMUM_ENTROPY)
      {
        all[n_considered].rule = i;
        all[n_considered].ent = ent;
        n_considered ++;
      }

      total_count++;
    }
  }

  fprintf(stderr, "Considered %d\n", n_considered);
  
  if (n_considered < n_best)
  {
    n_best = n_considered;
    fprintf(stderr, "Warning: Will return less rules(%d)\n", n_considered);
  }

  qsort(all, n_considered, sizeof(NODE), node_comparator);

  for (i = 0; i < n_best; ++i)
    printf("%u %u %g\n", i, all[i].rule, all[i].ent);
}

int main(int argc, char *argv[])
{
  FILE **f;
  int i;
  int n_files = argc - 3;

  if (argc < 4)
  {
    fprintf(stderr, "usage: %s lambda_bits N-best file0 file1 ...\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }

  MALLOC(f, sizeof(FILE*) * (n_files + 1));
  f[n_files] = NULL;

  fprintf (stderr, "%d files\n%d bits\n", n_files, atoi(argv[1]));


  for (i = 0; i < n_files; ++i)
  {
    char* name = argv[argc - 1 - i];

    if (!(f[i] = fopen(name, "r")))
    {
      fprintf(stderr, "Couldn't open %s for reading\n", name);
      exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Opened %s\n", name);
  }

  get_n_best(atoi(argv[1]), atoi(argv[2]), f);

  for (i = 0; i < n_files; ++i)
    fclose(f[i]);

  return EXIT_SUCCESS;
}
