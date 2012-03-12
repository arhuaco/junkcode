#ifndef __GA__POP__H__
#define __GA__POP__H__

#include <stdio.h>

/* Population of the Genetic Algotithm */

typedef struct GAPopulation GAPopulation;

/* create a population */
 GAPopulation * ga_population_create        (int pop_size, GAGene* sample);

/* destroy a population */
 void           ga_population_destroy       (GAPopulation *p);

/* initialize a population at random */
 void           ga_population_random_init   (GAPopulation *p);

/* evaluate the population */
 void           ga_population_eval          (GAPopulation *p, GAParams *prm);

/* reproduce the population */
void            ga_population_reproduce     (GAPopulation *p, GAParams* param);

/* the best score in the population */
 double         ga_population_best_score    (GAPopulation *p);

/* get the nth genome */
 GAGene *       ga_population_get_n         (GAPopulation *p, int n);

/* the average score in the population */
 double         ga_population_average_score (GAPopulation *p);

#endif
