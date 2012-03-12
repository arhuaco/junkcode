#ifndef __GA__1DGENOME__H__
#define __GA__1DGENOME__H__

#include <stdio.h>

typedef struct GA1DGenome GA1DGenome;

/* create a genome */
 GA1DGenome *    ga_1dgenome_new         (int size);

/* get the size of the genome */
 int             ga_1dgenome_size_get    (const GA1DGenome *g);

/* get the value of a gene (array cell) */
 int             ga_1dgenome_gene_get    (const GA1DGenome *g, int who);

/* init the genome at random */
void             ga_1dgenome_random_init (GA1DGenome *g);

/* is the genome unchanged since last evaluation?  */
 int             ga_1dgenome_fresh_get   (const GA1DGenome *g);

/* create a copy of a genome */
 GA1DGenome *    ga_1dgenome_dup         (const GA1DGenome *model);

/* destroy a genome */
 void            ga_1dgenome_destroy     (GA1DGenome *g);

/* initialize a genome */
 void            ga_1dgenome_init        (GA1DGenome *g, REAL param,
                                                         enum GAInitType type);
/* mutate a genome */
 void            ga_1dgenome_mutate      (GA1DGenome *g, REAL pmut,
                                          enum GAMutationType type);

/* single point crossover  */
void             ga_1dgenome_1point_cross(GA1DGenome *g1, GA1DGenome *g2);

/* get the genome fitness */
REAL             ga_1dgenome_fitness_get (const GA1DGenome *g);

/* set the genome fitness */
void             ga_1dgenome_fitness_set (GA1DGenome *g, REAL score);

/* sort an array of genomes */
void             ga_1dgenome_sort_desc (GA1DGenome **g, int size);

/* print the genome */
void             ga_1dgenome_print     (const GA1DGenome *g, FILE *f);

#endif
