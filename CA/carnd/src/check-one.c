#include <caiterate.h>
#include <util.h>

#include <stdio.h>
#include <stdlib.h>


#define PROGRAM_NAME "checkone"
#define DEBUG 1

float fitness(unsigned int rule, int N, int size, int is_random)
{
  CA *ca1;

  long double ent;

  ca1 = ca_create(size, rule);

  ca_rule_set(ca1, rule);

  if (is_random)
    ca_init_random(ca1);
  else
    ca_init_single(ca1); 
 
  ent = ca_entropy_measure(ca1, N, 4);

  return ent;
}

int main(int argc, char *argv[])
{

  unsigned rule;
  int N, size, is_random;

  if (argc != 5)
  {
    fprintf(stderr, "usage: %s rule N size random?\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }


  sscanf(argv[1], "%u", &rule);

  N          = atoi(argv[2]);
  size       = atoi(argv[3]);
  is_random  = atoi(argv[4]);

  if (DEBUG)
  {
    printf("(rule %u) (N %d) (size %d) (is_random %d)\n", rule, N, size, is_random);
  }
 
  printf("fitness = %g\n", fitness(rule, N, size, is_random));

  return EXIT_SUCCESS;
}
