#ifndef __CA__ITERATE__H__
#define __CA__ITERATE__H__

#include <stdio.h>

typedef struct CA CA;

/* Iterate the CA */
void        ca_iterate          (CA *ca, unsigned int steps);

/* Iterate the CA and measure the entropy in all cells
 * returning the minimum attained entropy */
long double ca_entropy_measure  (CA* ca, unsigned int nsteps, unsigned int H);

/* Create the cellular automaton */
CA *        ca_create           (unsigned int width, unsigned int rule);

/* Set the rule for the transition table */
void        ca_rule_set         (CA *ca, unsigned int rule);

/* Free the memory held by the cellular automaton */
void        ca_destroy          (CA *ca);

/* Initialize the CA grid using the systems "rand" function  */
void        ca_init_random      (CA *ca);  

/* Initialize the CA using a single cell in the center */
void    ca_init_single(CA *ca);  

/* Adds ca->width bits to the output buffer */
int     ca_to_file (CA *ca, int nbytes, FILE *p);

#endif
