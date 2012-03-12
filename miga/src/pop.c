#include <common.h>
#include <ga_random.h>
#include <1dgenome.h>
#include <ga_params.h>
#include <pop.h>


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ASSERT assert

struct GAPopulation
{
  GAGene   **pop;
  int        size;
  GAGene    *gene_best;
  double     score_best;
  double     score_avg;
};

GAPopulation *
ga_population_create (int pop_size, GAGene* sample)
{
  GAPopulation *new;
  int i;

  ASSERT(pop_size > 0);
  ASSERT(pop_size % 2 == 0); /* must be even */

  new = malloc(sizeof(GAPopulation));
  if(!new)
  {
    fprintf(stderr, "ga_population_create: out of memory");
    exit(1);
  }

  new->pop = malloc(sizeof(GAGene*) * pop_size);
  if(!new->pop)
  {
    fprintf(stderr, "ga_population_create: out of memory");
    exit(1);
  }

  for(i = 0; i < pop_size; ++i)
    new->pop[i] = ga_genome_dup(sample);

  new->size = pop_size;

  return new;
}

void
ga_population_destroy (GAPopulation *p)
{
  int i;

  ASSERT(p);
  ASSERT(p->pop);

  for(i = 0; i < p->size; ++i)
    ga_genome_destroy(p->pop[i]);

  free(p->pop);
  free(p);
}

void
ga_population_random_init (GAPopulation *p)
{
  int i;

  ASSERT(p);
  ASSERT(p->pop);

  for(i = 0; i < p->size; ++i)
    ga_genome_random_init(p->pop[i]);
}

void
ga_population_eval (GAPopulation *p, GAParams *prm)
{
  int i;

  ASSERT(p);
  ASSERT(p->pop);

  p->gene_best = NULL;
  p->score_avg = 0.0;

  for(i = 0; i < p->size; ++i)
  {
    REAL score;

    ASSERT(ga_params_evaluator_get(prm));

    if (ga_params_cachefitness_get(prm) && ga_genome_fresh_get(p->pop[i]))
      score = ga_genome_fitness_get(p->pop[i]);
    else
    {
      score = (ga_params_evaluator_get(prm))(p->pop[i]);
      ga_genome_fitness_set(p->pop[i], score);
    }

    p->score_avg += score;

    if(!p->gene_best || score > p->score_best)
    {
      p->gene_best = p->pop[i];
      p->score_best = score;
    }
  }

  ASSERT(p->size);
  p->score_avg /= (double) p->size;
}

/* We might use a temporal vector and binary search */

GAGene *
ga_population_roulette(GAPopulation *p, REAL total_fitness)
{
  ASSERT(p);
  ASSERT(total_fitness >= 0.0);
  REAL who, sum = 0.0;
  int i;

  who = random_float_get() * total_fitness;

  for (i = 0; i < p->size; ++i)
  {
    sum += ga_genome_fitness_get(p->pop[i]);

    if (who <= sum || p->size - 1 == i)
      break;
  }

  return p->pop[i];
}


void
ga_population_reproduce    (GAPopulation *p, GAParams* param)
{
  int i;
  int elite_size = ga_params_elitesize_get (param);
  REAL total_score = 0.0;

  for(i = 0; i < p->size; ++i)
    total_score +=  ga_genome_fitness_get(p->pop[i]);

  ASSERT(total_score >= 0.0);
  ASSERT(p);
  ASSERT(p->pop);

  ga_genome_sort_desc (p->pop, p->size);

  for(i = elite_size; i < p->size; ++i)
  {
    GAGene *old, *old2 = NULL; /* might point to the same genome! */
    GAGene *new, *new2;

    old = p->pop[i];
    new = ga_genome_dup(ga_population_roulette(p, total_score));

    if (i < p->size - 1) /* can produce 2 genomes in this run */
      if (random_float_get() < ga_params_pcross_get(param))
      {
        old2 = p->pop[i + 1];
        new2 = ga_genome_dup(ga_population_roulette(p, total_score));
      }

    ga_genome_destroy(old);

    if (old2 && old != old2)
      ga_genome_destroy(old2);

    p->pop[i] = new;

    if (old2)
      p->pop[i + 1] = new2;

    if (old2 && old != old2)
      ga_genome_1point_cross(p->pop[i], p->pop[i + 1]);

    ga_genome_mutate(p->pop[i], ga_params_pmut_get(param), GA_MUTATION_UNIFORM);

    if (old2)
    {
      ga_genome_mutate(p->pop[i + 1], ga_params_pmut_get(param), GA_MUTATION_UNIFORM);
      ++i;
    }
  }
}


double
ga_population_best_score (GAPopulation *p)
{
  ASSERT(p);
  ASSERT(p->gene_best);

  return p->score_best;
}

GAGene *
ga_population_get_n (GAPopulation *p, int n)
{
  ASSERT(p);
  ASSERT(p->pop);
  ASSERT(n >= 0 && n < p->size);

  return p->pop[n];
}

double
ga_population_average_score (GAPopulation *p)
{
  ASSERT(p);

  return p->score_avg;
}
