#include <caiterate.h>
#include <util.h>

#include <stdlib.h>

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

char
ca_cell_get (CA *ca, unsigned int pos)
{
  ASSERT(ca);
  ASSERT(pos < ca->width);

  return ca->cell[pos];
}

void
ca_cell_set (CA *ca, unsigned int pos, char value)
{
  ASSERT(ca);
  ASSERT(pos < ca->width);
  ASSERT(value == 0 || value == 1);

  ca->cell[pos] = value;
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
ca_init_single(CA *ca)
{
  unsigned int i;
  
  for (i = 0; i < ca->width; i++)
  {
      ca->cell[i] = 0;
  }
  
  ca->cell[ca->width/2] = 1;
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
