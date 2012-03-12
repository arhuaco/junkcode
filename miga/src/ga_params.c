#include <common.h>
#include <1dgenome.h>
#include <ga_params.h>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>

#define ASSERT assert


struct GAParams
{
  /* mutation probability */
  float                 pmut;

  /* crossover probability */
  float                 pcross;

  /* size of the elite */
  unsigned int         elite_size;

  /* shall we cache the fitness function? */
  int                  cache_fitness;

  /* the evaluation function */
  GAEvaluator          evaluator;
};

GAParams *
ga_params_new (void)
{
  GAParams *new;

  new = malloc (sizeof(GAParams));
  if(!new)
  {
    fprintf(stderr, "ga_params_new: out of memory");
    exit(1);
  }

  new->evaluator  = NULL;
  new->pmut       = 0.1;
  new->pcross     = 0.8;
  new->elite_size = 1;
  new->cache_fitness = 1;

  return new;
}

void
ga_params_pmut_set (GAParams* p, float pmut)
{
  ASSERT(p);
  p->pmut = pmut;
}

void
ga_params_pcross_set (GAParams* p, float pcross)
{
  ASSERT(p);
  p->pcross = pcross;
}

void
ga_params_elitesize_set (GAParams* p, unsigned int size)
{
  ASSERT(p);
  p->elite_size = size;
}

void
ga_params_cachefitness_set (GAParams* p, unsigned int cache)
{
  ASSERT(p);
  p->cache_fitness = cache;
}

void
ga_params_delete (GAParams *p)
{
  ASSERT(p);
  free(p);
}

float
ga_params_pmut_get (GAParams *p)
{
  ASSERT(p);
  return p->pmut;
}

float
ga_params_pcross_get (GAParams *p)
{
  ASSERT(p);
  return p->pcross;
}

unsigned int
ga_params_elitesize_get  (GAParams *p)
{
  ASSERT(p);
  return p->elite_size;
}

unsigned int ga_params_cachefitness_get (GAParams* p)
{
  ASSERT(p);
  return p->cache_fitness;
}

GAEvaluator
ga_params_evaluator_get (GAParams *p)
{
  ASSERT(p);
  ASSERT(p->evaluator);

  return p->evaluator;
}

void
ga_params_evaluator_set (GAParams *p, GAEvaluator f)
{
  ASSERT(p);
  ASSERT(f);

  p->evaluator = f;
}
