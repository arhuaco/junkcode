#include <common.h>
#include <1dgenome.h>
#include <ga_params.h>
#include <pop.h>

#include <stdlib.h>
#include <time.h>

int
ga_run (FILE * output_file, GAParams* param, int iterations, GAPopulation *pop)
{
  int i;

  if (!ga_params_evaluator_get(param))
  {
    fprintf(stderr, "No evaluator set in param\n");
    return EXIT_FAILURE;
  }

  ga_population_eval(pop, param);

  for(i = 0; i < iterations; ++i)
  {
    double score;

    ga_population_reproduce(pop, param);
    ga_population_eval(pop, param);

    score = ga_population_best_score(pop);

    fprintf(output_file, "[generat:%d][avg:%g][best:%g]\n", i,
            ga_population_average_score(pop), ga_population_best_score(pop));

    fflush(output_file);
  }

  return EXIT_SUCCESS;
}
