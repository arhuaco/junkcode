#include <miga/miga.h>
#include <neural-net.h>

/*
 * This is an experiment ! It worked about 6 years ago,
 * but it requieres too much training.
 *
 * Don't rely in this expriment for now.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>

/* Number of units in the neural net */
#define NEURAL_NET_SIZE 32
/* Initial state of the unit's memory */
#define UNIT_MEMORY_INITIAL_STATE 0
/* Number of bits of an evolved weight + 1 */
#define NEURAL_NET_P_LUS_1 8
/* size of the genome */
#define GA_GENOME_SIZE (NEURAL_NET_P_LUS_1 * NEURAL_NET_SIZE * NEURAL_NET_SIZE)

/* convert a genome to the real weights */
void
weights_fill (const GA1DGenome *g, double wgt[])
{
  int i;
  int size;

  assert(NEURAL_NET_P_LUS_1 == 8);

  size = ga_1dgenome_size_get(g);

  for(i = 0; i < size; i += NEURAL_NET_P_LUS_1)
  {
    double w = 0.0;

    if(ga_1dgenome_gene_get(g, i + 1))
      w += 0.5;
    if(ga_1dgenome_gene_get(g, i + 2))
      w += 0.25;
    if(ga_1dgenome_gene_get(g, i + 3))
      w += 0.125;
    if(ga_1dgenome_gene_get(g, i + 4))
      w += 0.0625;
    if(ga_1dgenome_gene_get(g, i + 5))
      w += 0.03125;
    if(ga_1dgenome_gene_get(g, i + 6))
      w += 0.015625;
    if(ga_1dgenome_gene_get(g, i + 7))
      w += 0.0078125;
    if(ga_1dgenome_gene_get(g, i))
      w *= -1; /* sign */
    assert(fabs(w) < 0.995);

    wgt[i / NEURAL_NET_P_LUS_1] = w; /* max(abs(w)) == 0.9921875 */
  }
}


NeuralNet *net = NULL;

REAL
simple_evaluator(GA1DGenome* g) /* TODO: make this const GA1Genome * */
{
  int i;
  static double weight [NEURAL_NET_SIZE * NEURAL_NET_SIZE];
  double score = 0.0;

  weights_fill (g, weight);

  if (net == NULL)
    net = neural_net_new(NEURAL_NET_SIZE); /* TODO: reuse */

  neural_net_init(net, UNIT_MEMORY_INITIAL_STATE);

  for(i = 0; i < NEURAL_NET_SIZE; ++i)
    neural_net_persistent_input_set(net, i, 0.5);

  for(i = 0; i <= 120; ++i)
  {
    double out;
    double target;

    neural_net_iterate(net, weight);

    out = neural_net_output_get(net, 0); /* get the output from the unit 0 */

    /* You might want to use a cache */
    target = sin((2.0 * M_PI / 120.0) * i);

    score += (target - out) * (target - out);
  }

  //fprintf(stderr, "[error_sq:%g]", score);

  if(score != 0.0)
    score = 1.0/ score;
  else
    score = 1e20; /* solution found? might never happen */

  //fprintf(stderr, "[score:%g]\n", score);

  //neural_net_destroy(net);

  return score;
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

  sample = ga_1dgenome_new(GA_GENOME_SIZE);
  pop = ga_population_create(200, sample); /* 1000 individuals! */
  ga_1dgenome_destroy(sample);

  ga_population_random_init(pop);

  params = ga_params_new();
  ga_params_evaluator_set     (params, simple_evaluator);
  ga_params_pmut_set          (params, 0.01);
  ga_params_pcross_set        (params, 0.5);
  ga_params_elitesize_set     (params, 5);
  ga_params_cachefitness_set  (params, 1);

  seed = time(NULL);
  printf("[seed:%ld]\n", seed);

  status = ga_run(stdout, params, 1000, pop);
  for (i = 0; i < 5; ++i)
    printf("Best:%d: %g", i, simple_evaluator(ga_population_get_n(pop, i)));

  ga_population_destroy(pop);

  exit (status);
}
