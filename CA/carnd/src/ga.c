#include <miga/miga.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fitness.h>

inline unsigned int
genome2rule(GA1DGenome* g)
{
  int size;
  int i;
  unsigned int rule = 0;

  size = ga_1dgenome_size_get(g);

  for (i = 0; i < size; ++i)
    rule |=  ga_1dgenome_gene_get(g, i) << (31 - i);

  return rule;
}

REAL
min_entropy(GA1DGenome* g)
{
 return fitness(genome2rule(g));
}

#define POP_SIZE 100
#define ITERATIONS 100
#define ITERATIONS2 300

int
main (int argc, char **argv)
{
  long seed;
  int status;
  int i;
  GAParams *params;
  GA1DGenome *sample;
  GAPopulation *pop;
  FILE *p;

  seed = time(NULL);
  random_seed_set(time(NULL));

  p = fopen("bog.dat", "w");

  if (!p)
  {
    fprintf(stderr, "Couldn't open bog.dat\n");
    return EXIT_FAILURE;
  }

  fprintf(p, "[seed:%ld]\n", seed);

  sample = ga_1dgenome_new(32);
  pop = ga_population_create(POP_SIZE, sample);
  ga_1dgenome_destroy(sample);
  ga_population_random_init(pop);

  params = ga_params_new();
  ga_params_evaluator_set     (params, min_entropy);
  ga_params_pmut_set          (params, 0.5);
  ga_params_pcross_set          (params, 1);
  ga_params_elitesize_set     (params, 1);
  ga_params_cachefitness_set  (params, 1);


  status = ga_run(p, params, ITERATIONS, pop);

  for (i = 0; i < 5; ++i)
    ga_1dgenome_print(ga_population_get_n(pop, i), p);

  ga_params_pmut_set          (params, 0.01);

  status = ga_run(p, params, ITERATIONS2, pop);

  for (i = 0; i < 5; ++i)
    ga_1dgenome_print(ga_population_get_n(pop, i), p);


  ga_population_destroy(pop);

  exit (status);
}
