#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <ca-utils.h>
#include <ca1dr2.h>

/*
 * This program creates a graph with all the possible configuration of an
 * 1-dimensional r=2 CA that can be read by http://www.graphviz.org/.
 * You can use graphviz to create the images.
 *
 * Usage example:
 * ./cagraph 1718195561 5
 * dot -Tpng graph-rule1718195561-n5.dot > graph-rule1718195561-n5.png
 *
 * You can get some CA rules here:
 * http://arhuaco.blogspot.com/2005/07/prng-with-one-dimensional-r1-cellular.html
 *
 */

#define PROGRAM_NAME "cagraph"
#define DEBUG 1

int
main(int argc, char *argv[])
{
  int i;

  int N;
  int NSTATES;

  unsigned int rule;
  CA *ca;

  FILE *p;
  char filename[256];

  if (argc != 3)
  {
    fprintf(stderr, "usage: %s rule ca-size\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }

  sscanf(argv[1], "%u", &rule);

  sscanf(argv[2], "%d", &N);

  if (DEBUG)
    fprintf(stderr, "rule %u\nN=%d\n", rule, N);

  ca = ca_create(N, rule);

  sprintf(filename, "graph-rule%u-n%d.dot", rule, N);
  filename[255] = 0;

  p = fopen(filename, "w");

  if (!p)
  {
    fprintf(stderr, "couldn't open file '%s'\n", filename);
    exit(EXIT_FAILURE);
  }

  fprintf(p, "digraph Graph_rule%u_n%d{\n", rule, N);

  NSTATES =  (2 << (N - 1));

  for (i = 0; i < NSTATES; ++i)
  {
    int dest;

    ca_int_init(ca, i);
    ca_iterate(ca, 1);
    dest = ca_int_get(ca);

    fprintf(p, " %d -> %d;\n", i, dest);
  }

  fprintf(p, "}\n");

  fclose(p);

  ca_destroy(ca);

  return EXIT_SUCCESS;
}
