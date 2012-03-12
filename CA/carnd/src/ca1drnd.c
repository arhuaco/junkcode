/* vim: set et ts=2 */
/* I compiled with : gcc -O2 -ansi -Wall -pedantic ca1drnd.c -o ca1rnd */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define PROGRAM_NAME "ca1drnd"

#define DEBUG 1

#define   MALLOC(var,size) {\
            (var) = malloc(size); \
       	    if(!(var)) \
	            fprintf(stderr, "%s:%d: out of memory\n", __FILE__, __LINE__), \
	            exit(EXIT_FAILURE); } \

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

/* Adds ca->width bits to the output buffer */
int         ca_to_file (CA *ca, int nbytes, FILE *p);

int
main(int argc, char *argv[])
{
  CA *ca;
  char *filename = "out.rnd";
  FILE *p;
  int nbytes;
  unsigned rule;

  srand(time(NULL));

  nbytes = 1024*1024*10;

  if (argc != 2)
  {
    fprintf(stderr, "usage: %s rule\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }
  else
  {
    char *strend;

    rule = strtoul(argv[1], &strend, 10);

    if (*strend)
    {
      fprintf(stderr, "Invalid rule : '%s'\n", argv[1]);
      exit(EXIT_FAILURE);
    }
  }

  if (DEBUG)
  {
    printf("rule : %u\n", rule);
  }

  ca = ca_create(512, rule);

  if (!(p = fopen(filename, "wb")))
  {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

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


#define ROW_SIZE 16

int
ca_to_file (CA *ca, int nbytes, FILE *p)
{
  static int CACHE_VAR = 0;
  static int CACHE_COUNT = 0;
  static char **buff = NULL;
  static int row_count;
  int i;

  ASSERT(ca);
  ASSERT(p);

  if (!buff)
  {
    MALLOC(buff, sizeof(char*)*ROW_SIZE);

    for (i = 0; i < ROW_SIZE; ++i)
      MALLOC(buff[i], ca->width);

    row_count = 0;
  }

  ASSERT(buff);

  if(row_count < ROW_SIZE)
  {
    for (i = 0; i < ca->width ; ++i)
      buff[row_count][i] = ca->cell[i];

    row_count ++;
  }
  else if (row_count == ROW_SIZE)
  {
    int r;

    for (i = 0; i < ca->width ;++i)
    {
      for (r = 0; r < ROW_SIZE; ++r)
      {
        CACHE_VAR |= buff[r][i] << CACHE_COUNT;
        CACHE_COUNT++;

        if (CACHE_COUNT == sizeof (int) * 8)
        {
          int w = fwrite (&CACHE_VAR, sizeof (int), 1, p);

          if (w != 1)
          {
            fprintf(stderr, "Couldn't write word!\n"); 
            exit(EXIT_FAILURE);
          }

          CACHE_COUNT = 0;
          CACHE_VAR = 0;
          nbytes -= sizeof (int);
        }
      }
    }

    row_count = 0;
  }
  else
    ASSERT(NULL);

  return nbytes;
}
