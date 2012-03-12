/* vim: set et ts=2 */
/* I compiled with : gcc -O2 -ansi -Wall -pedantic ca1drnd.c -o ca1rnd */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_NAME "ca1drnd"

#define DEBUG 1

#define   MALLOC(var,size) \
            (var) = malloc(size); \
       	    if(!(var)) \
	            fprintf(stderr, "%s:%d: out of memory\n", __FILE__, __LINE__), \
	            exit(EXIT_FAILURE); \

#define   ASSERT(x) \
            if(!(x)) \
              fprintf(stderr, "%s:%d: ASSERT failed\n", __FILE__, __LINE__), \
	          exit(EXIT_FAILURE);

typedef struct CA CA;

/* Iterate the CA */
void        ca_iterate          (CA *ca, unsigned int steps);

/* Create the cellular automaton */
CA *        ca_create           (unsigned int width, unsigned int rule);

/* Set the rule for the transition table */
void        ca_rule_set         (CA *ca, unsigned int rule);

/* Free the memory held by the cellular automaton */
void        ca_destroy          (CA *ca);

/* Initialize the CA grid using the system's "rand" function  */
void        ca_init_random      (CA *ca);  

/* Give feedback from a CA to another  */
void        ca_feedback         (CA *ca_dst, CA *ca_src, unsigned spacing);  

/* Adds ca->width bits to the output buffer */
long long         ca_to_file (CA *ca, long long nbytes, FILE *p);

int
main(int argc, char *argv[])
{
  CA **ca;
  char *filename = "out.rnd";
  FILE *p;
  long long nbytes;
  int nrules;
  unsigned int *rules;
  int j;

  srand(time(NULL));

  nbytes = 1024L * 1024L * 1024L * 2L;

  nrules = argc - 1;

  if (nrules < 1)
  {
    fprintf(stderr, "usage: %s rule0 rule1 ... ruleN\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }
  else
  {
    char *strend;
    int i;

    MALLOC(rules, sizeof(unsigned int) * nrules);
    MALLOC(ca, sizeof(CA*) * nrules);

    for (i = 0; i < nrules; ++i)
    {
      rules[i] = strtoul(argv[i + 1], &strend, 10);

      if (*strend)
      {
        fprintf(stderr, "Invalid rule : '%s'\n", argv[i + 1]);
        exit(EXIT_FAILURE);
      }

      if (DEBUG)
        fprintf(stderr, "Parsed rule '%s'\n", argv[i + 1]);

      ca[i] = ca_create(503, rules[i]);
      ca_init_random(ca[i]);
    }
  }


  if (!(p = fopen(filename, "wb")))
  {
    perror("fopen");
    exit(EXIT_FAILURE);
  }


  while (nbytes >= 0)
  {
    int i;

    for (i = nrules - 1; i >= 1 ; --i)
      ca_feedback(ca[i], ca[i - 1], 4);

    for (i = 0; i < nrules; ++i)
      ca_iterate(ca[i], 1);

    nbytes = ca_to_file(ca[nrules - 1], nbytes, p);
  }
  
  fclose(p);

  for (j = 0; j < nrules; ++j)
    ca_destroy(ca[j]);
        
  return EXIT_SUCCESS;
}

struct CA
{
  unsigned int width;
  unsigned int rule;
  char *cell;
};

CA *
ca_create(unsigned int width, unsigned int rule)
{
  CA *new;

  MALLOC(new, sizeof(CA));
  
  new->rule  = rule;
  new->width = width;
  
  MALLOC(new->cell, width + 2); 

  return new;
}

void
ca_rule_set(CA *ca, unsigned int rule)
{
  ASSERT(ca);
  ASSERT(ca->cell);
  ca->rule = rule;
}
      
void
ca_destroy(CA *ca)
{
  ASSERT(ca);
  ASSERT(ca->cell);
  
  free(ca->cell);
  free(ca);
}

void
ca_init_random(CA *ca)
{
  unsigned int i;
  
  for (i = 0; i < ca->width; i++)
  {
      char n = rand() % 2;
      ca->cell[i] = n;
  }
}

void
ca_feedback(CA *ca_dst, CA *ca_src, unsigned spacing)
{
  int i;

  for (i = 0; i < ca_src->width; i += spacing)
    ca_dst->cell[i] = ca_src->cell[i];
}

/*
 * Now we iterate the CA inplace.
 * I sould've done this a long time ago.
 *
 * The ca array _must_ have two extra locations.
 */

void ca_iterate (CA *ca, unsigned int steps)
{
  unsigned int i, col, lookup;

  ASSERT(ca);
  ASSERT(steps);
  ASSERT(ca->cell);
  ASSERT(ca->width > 10);

  for (i = 0; i < steps; i++)
  {
     ca->cell[ca->width]     = ca->cell[0];
     ca->cell[ca->width + 1] = ca->cell[1];
     
     lookup = ca->cell[ca->width - 2] << 3 |
              ca->cell[ca->width - 1] << 2 |
              ca->cell[0] << 1 |
              ca->cell[1];

     for (col = 0; col < ca->width; col++)
     {
        lookup = (lookup << 1 | ca->cell[col + 2]) & 0x001F;

        ASSERT(lookup >=0 && lookup <= 31);
        
        ca->cell[col] = (ca->rule & (1 << lookup)) >> lookup;
     }
  }
}

/*  returns (nbytes - bytes_written) */

/* TODO: Using static variables to keep the state. Good for now. */

long long
ca_to_file (CA *ca, long long nbytes, FILE *p)
{
  static int CACHE_VAR = 0;
  static int CACHE_COUNT = 0;

  int i;

  ASSERT(ca);
  ASSERT(p);
 
  for (i = 0; i < ca->width ; i += 2)
  {
    CACHE_VAR |= ca->cell[i] << CACHE_COUNT;
    CACHE_COUNT++;

    if (CACHE_COUNT == sizeof (int) * 8)
    {
      fwrite (&CACHE_VAR, sizeof (int), 1, p);
      CACHE_COUNT = 0;
      CACHE_VAR = 0;
      nbytes -= sizeof (int);
    }
  }

  return nbytes;
}
