#include <miga/miga.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * "alternate" Example.
 *
 * The idea is to get a genome with alternating 0s and 1s.
 *
 */

REAL
simple_evaluator(GA1DGenome* g)
{
  int size;
  int i;
  REAL count = 0.0;

  size = ga_1dgenome_size_get(g);

  for (i = 0; i < size; ++i)
    if (ga_1dgenome_gene_get(g, i) == (i % 2))
      count += 1;

  return count;
}

#define POP_SIZE 100
#define GENOME_SIZE 100

int
main (int argc, char **argv)
{
  long seed;
  int status;
  int i;
  GAParams *params;
  GA1DGenome *sample;
  GAPopulation *pop;

  random_seed_set(time(NULL));

  sample = ga_1dgenome_new(20);
  pop = ga_population_create(GENOME_SIZE, sample);
  ga_1dgenome_destroy(sample);
  ga_population_random_init(pop);

  params = ga_params_new();
  ga_params_evaluator_set     (params, simple_evaluator);
  ga_params_pmut_set          (params, 0.1);
  ga_params_pcross_set        (params, 1);
  ga_params_elitesize_set     (params, 1);
  ga_params_cachefitness_set  (params, 1);

  seed = time(NULL);
  printf("[seed:%ld]\n", seed);

  status = ga_run(stdout, params, 100, pop);

  for (i = 0; i < 5; ++i)
    ga_1dgenome_print(ga_population_get_n(pop, i), stdout);


  ga_population_destroy(pop);

  exit (status);
}
