#ifndef __GA__COMMON__H__
#define __GA__COMMON__H__

enum GAInitType      {GA_INIT_RANDOM_UNIFORM = 1};
enum GAMutationType  {GA_MUTATION_UNIFORM = 1};

#define REAL float

#define DEBUG /* enable asserts */

#define GAGene GA1DGenome
#define ga_genome_new ga_1dgenome_new
#define ga_genome_dup ga_1dgenome_dup
#define ga_genome_mutate ga_1dgenome_mutate
#define ga_genome_destroy ga_1dgenome_destroy
#define ga_genome_fitness_get ga_1dgenome_fitness_get
#define ga_genome_fitness_set ga_1dgenome_fitness_set
#define ga_genome_random_init ga_1dgenome_random_init
#define ga_genome_sort_desc ga_1dgenome_sort_desc
#define ga_genome_fresh_get ga_1dgenome_fresh_get
#define ga_genome_1point_cross ga_1dgenome_1point_cross

#endif
