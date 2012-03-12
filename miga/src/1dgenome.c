#include <common.h>
#include <1dgenome.h>
#include <ga_random.h>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define ASSERT assert

struct GA1DGenome
{
  /* the bitstring */
  char      *gene;

  /* size of the genome*/
  int        size;

  /* result of the evaluation of the fitness */
  REAL       score;

  /* false if the gene changes after an evaluation
   * don't forget to change the 'fresh' flag whenever
   * you change the genome */
  char       fresh;
};

GA1DGenome *
ga_1dgenome_new (int size)
{
  GA1DGenome *new;
  char   *genome;

  ASSERT(size>0);

  genome = malloc (sizeof(char)*size);
  if(!genome)
  {
    fprintf(stderr,"ga_gene_new: out of memory");
    exit(1);
  }

  new = malloc (sizeof(GA1DGenome));
  if(!new)
  {
    fprintf(stderr,"ga_gene_new: out of memory");
    exit(1);
  }

  new->gene  = genome;
  new->size  = size;
  new->fresh = 0;
  new->score = -1;

  return new;
}

int
ga_1dgenome_size_get (const GA1DGenome *g)
{
  ASSERT(g);

  return g->size;
}

int
ga_1dgenome_gene_get (const GA1DGenome *g, int who)
{
  ASSERT(g);
  ASSERT(who < g->size && who >= 0);

  return g->gene[who];
}

GA1DGenome *
ga_1dgenome_dup (const GA1DGenome *model)
{
  GA1DGenome *dup;

  ASSERT(model);
  ASSERT(model->gene);
  ASSERT(model->size > 0);

  dup = ga_1dgenome_new(model->size);

  dup->size    = model->size;
  dup->score   = model->score;
  dup->fresh   = model->fresh;

  memcpy(dup->gene, model->gene, model->size);

  return dup;
}

void
ga_1dgenome_destroy (GA1DGenome * g)
{
  ASSERT(g);
  ASSERT(g->gene);

  free(g->gene);
  free(g);
}


void
ga_1dgenome_fitness_set (GA1DGenome *g, REAL score)
{
  ASSERT(g);
  ASSERT(g->gene);
  ASSERT(g->size > 0);

  g->score = score;
  g->fresh = 1;
}

int
ga_1dgenome_fresh_get (const GA1DGenome *g)
{
  ASSERT(g);
  ASSERT(g->gene);

  return  g->fresh;
}


REAL
ga_1dgenome_fitness_get (const GA1DGenome *g)
{
  ASSERT(g);
  /* ASSERT(g->fresh); Roulette selection need the fitness */

  return g->score;
}

void
ga_1dgenome_random_init (GA1DGenome *g)
{
  int i;

  ASSERT(g);
  ASSERT(g->gene);

  for(i = 0; i < g->size; ++i)
  {
    g->gene[i] = random_int_get(2);

    ASSERT(g->gene[i]==0 || g->gene[i]==1);
  }

  g->fresh = 0;
}

static int
ga_1dgenome_comparator (const void *a, const void *b)
{
  REAL sa, sb;

  sa = ga_1dgenome_fitness_get(*((const GA1DGenome **)a));
  sb = ga_1dgenome_fitness_get(*((const GA1DGenome **)b));

  if(sa > sb) /* the comparison is inverted */
    return -1;

  if(sa < sb)
    return 1;

  return 0;
}

void
ga_1dgenome_sort_desc (GA1DGenome **g, int size)
{
  qsort(g, size, sizeof(GA1DGenome*), ga_1dgenome_comparator);
}

void
ga_1dgenome_print (const GA1DGenome *g, FILE *f)
{
  int i;

  ASSERT(g);
  ASSERT(g->gene);

  fprintf(f, "genome:'");

  for(i = 0; i < g->size; ++i)
  {
    fprintf(f, "%d", g->gene[i]);
  }

  fprintf(f, "'\n");
}

/*
 * Note that a crossover will always take place. We avoid just swaps.
 */

void
ga_1dgenome_1point_cross (GA1DGenome *g1, GA1DGenome *g2)
{
  int cross_point;
  int i;

  ASSERT(g1);
  ASSERT(g1->gene);
  ASSERT(g1->size > 4);
  ASSERT(g2);
  ASSERT(g2->gene);
  ASSERT(g2->size == g1->size);

  cross_point = 1 + random_int_get(g1->size - 2);
  ASSERT(cross_point < g1->size - 1);

  for (i = cross_point; i < g1->size; ++i)
  {
    char tmp = g1->gene[i];
    g1->gene[i] = g2->gene[i];
    g2->gene[i] = tmp;
  }

  g1->fresh = 0;
  g2->fresh = 0;
}

void
ga_1dgenome_mutate (GA1DGenome *g, REAL pmut, enum GAMutationType type)
{
  int i;
  REAL nmut;

  ASSERT(pmut < 1.0);
  ASSERT(pmut > 0.0);
  ASSERT(g);
  ASSERT(g->gene);
  ASSERT(g->size > 0);

  nmut = pmut * (REAL) g->size;

  if(nmut < 1.0)
  {
    int actually_mutated = 0;

    for(i = 0; i < g->size; ++i)
      if(pmut >= random_float_get())
      {
        g->gene[i] = g->gene[i] ? 0 : 1;

        actually_mutated++;
      }

    nmut = actually_mutated;
  }
  else
  {
    int n;
    int who;

    n = ceil(nmut);
    nmut = n;

    for(i = 0; i < n; ++i)
    {
      who = random_int_get(g->size);
      ASSERT(who>=0 && who < g->size);

      g->gene[who] = g->gene[who] ? 0 : 1;
    }
  }

  if(nmut > 0)
    g->fresh = 0;
}
