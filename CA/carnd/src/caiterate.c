#include <caiterate.h>
#include <entropy_meter.h>
#include <util.h>

#include <stdio.h>
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

/* This shouldn't work on a multi-threaded program */

static EntropyMeter **em = 0;
static int em_lastsize = 0;
static int em_lasth = 0;

long double
ca_entropy_measure (CA* ca, unsigned int nsteps, unsigned int H)
{
  int i;
  long double min_ent;

  if (!em || em_lastsize < ca->width || em_lasth != H)
  {
    if(em)
    {
      for (i = 0; i < em_lastsize; ++i)
        entropy_meter_destroy(em[i]);
      free(em);
    }

    MALLOC(em, sizeof(EntropyMeter *) * ca->width);

    for (i = 0; i < ca->width; ++i)
      em[i] = entropy_meter_new(H);

    em_lastsize = ca->width;
    em_lasth = H;
  }
  else
  {
    ASSERT(em_lastsize >= ca->width);

    for (i = 0; i < ca->width; ++i)
      entropy_meter_reset(em[i]);
  }

  for (i = 0; i < nsteps; ++i)
  {
    int pos;

    ca_iterate (ca, 1);

    for (pos = 0; pos < ca->width; ++pos)
      entropy_meter_count (em[pos], ca->cell[pos]);
  }

  min_ent = (long double) H;

  for (i = 0; i < ca->width; ++i)
  {
    long double ent;

    ent = entropy_meter_get_measure(em[i]);

    if (ent < min_ent)
      min_ent = ent;
  }

  return min_ent;
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

/* TODO: Using static variables to keep the state */

int
ca_to_file (CA *ca, int nbytes, FILE *p)
{
  static int CACHE_VAR = 0;
  static int CACHE_COUNT = 0;

  int i;

  ASSERT(ca);
  ASSERT(p);

  for (i = 0; i < ca->width ;++i)
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
