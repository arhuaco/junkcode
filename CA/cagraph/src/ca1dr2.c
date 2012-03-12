#include <stdio.h>
#include <stdlib.h>
#include <ca-utils.h>

typedef struct CA CA;

/* Iterate the CA */
void          ca_iterate          (CA *ca, unsigned int steps);

/* Create the cellular automaton */
CA *          ca_create           (unsigned int width, unsigned int rule);

/* Set the rule for the transition table */
void          ca_rule_set         (CA *ca, unsigned int rule);

/* Free the memory held by the cellular automaton */
void          ca_destroy          (CA *ca);

/* Print the cellular automaton */
void          ca_print            (CA *ca);

/* Copy the initial configuration from an integer */
void          ca_int_init         (CA *ca, int conf);

/* Get the integer of a CA configuration */
unsigned int  ca_int_get          (CA *ca);

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
ca_print(CA *ca)
{
  unsigned int i;
  
  for (i = 0; i < ca->width; i++)
  {
      printf("%c", '0' + ca->cell[i]);
  }
}

void     ca_int_init            (CA *ca, int conf)
{
  int i;
 
  /* We need to itereate when conf == 0, too */
  for (i = ca->width - 1; i >= 0; i--)
  {
    ca->cell[i] = conf % 2;
    conf /= 2;
  }
}

unsigned int  ca_int_get          (CA *ca)
{
  int i;
  int sum = 0;
  int mul = 1;
 
  for (i = ca->width - 1; i >= 0; i--)
  {
    if (ca->cell[i])
      sum += mul;

    mul *= 2;
  }

  return sum;
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
  ASSERT(ca->width > 2);

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
        lookup = (lookup << 1 | ca->cell[col + 2]) & 0x1F;

        ASSERT(lookup >=0 && lookup <= 31);
        
        ca->cell[col] = (ca->rule & (1 << lookup)) >> lookup;
     }
  }
}
