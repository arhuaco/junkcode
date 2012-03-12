#ifndef __CA__ITERATE__H__
#define __CA__ITERATE__H__

typedef struct CA CA;


/* Create the cellular automaton */
CA *        ca_create           (unsigned int width, unsigned int rule);

/* Set the rule for the transition table */
void        ca_rule_set         (CA *ca, unsigned int rule);

/* Free the memory held by the cellular automaton */
void        ca_destroy          (CA *ca);

/* Initialize the CA grid using the systems "rand" function  */
void        ca_init_random      (CA *ca);  

/* Initialize the CA using a single cell in the center */
void        ca_init_single(CA *ca); 

/* Get the value of a cell */
char        ca_cell_get          (CA *ca, unsigned int pos);

/* Set the value of a cell */
void        ca_cell_set          (CA *ca, unsigned int pos, char value);

/* Iterate the CA */
void        ca_iterate          (CA *ca, unsigned int steps);

#endif
