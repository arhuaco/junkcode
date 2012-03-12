typedef struct CA CA;

/* Iterate the CA */
void          ca_iterate          (CA *ca, unsigned int steps);

/* Create the cellular automaton */
CA *          ca_create           (unsigned int width, unsigned int rule);

/* Set the rule for the transition table */
void          ca_rule_set         (CA *ca, unsigned int rule);

/* Free the memory held by the cellular automaton */
void          ca_destroy          (CA *ca);

/* Print the cellular automaton */
void          ca_print            (CA *ca);

/* Copy the initial configuration from an integer */
void          ca_int_init         (CA *ca, int conf);

/* Get the integer of a CA configuration */
unsigned int  ca_int_get          (CA *ca);

struct CA
{
  unsigned int width;
  unsigned int rule;
  char *cell;
};
