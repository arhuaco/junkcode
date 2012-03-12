#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <ca-utils.h>
#include <ca1dr2.h>

#define PROGRAM_NAME "ca-findcycles"
#define DEBUG 1

#define UINT unsigned long long
#define UFM  "llu"

/*
 * This program finds all cycles in a given 1-dimensional CA or r=2
 *
 * Usage example:
 * ./ca-findcycles 1718195561 5
 *
 * You can get some CA rules here:
 * http://arhuaco.blogspot.com/2005/07/prng-with-one-dimensional-r1-cellular.html
 *
 */

char *create_graph (UINT size)
{
  char *new;
  UINT  nbytes =  size / 4;

  ASSERT(!(size % 4));

  MALLOC(new, nbytes);

  if (DEBUG)
    fprintf(stderr, "Memory = %" UFM "B %" UFM  "MB\n", nbytes, nbytes / (1024 * 1024));

  return new;
}

void
free_graph (char *g)
{
  ASSERT(g);
  free(g);
}

#define STATE_HAS_BEEN_VISITED      0x1
#define STATE_IS_BEING_VISITED      0x2
#define STATE_ALL_SET_MASK          0x3

void flags_set (char *graph, UINT state, char flags)
{
  graph[state / 4] |= flags << (state % 4 * 2);
}

int flags_test (char *graph, UINT state, char flags)
{
  return (flags << (state % 4 * 2)) & graph[state / 4];
}

void flags_unset (char *graph, UINT state, char flags)
{
  graph[state / 4] &= ~(flags << (state % 4 * 2));
}

UINT next_state (CA *ca, UINT state)
{
   ca_int_init(ca, state);
   ca_iterate(ca, 1);
   return ca_int_get(ca);
}

void search_cycle(CA *ca, char *graph, UINT starting_state)
{
  UINT actual_state = starting_state;
  int is_new_cycle = 0;

  while (!flags_test(graph, actual_state, STATE_HAS_BEEN_VISITED))
  {
    flags_set(graph, actual_state, STATE_HAS_BEEN_VISITED | STATE_IS_BEING_VISITED);
    actual_state = next_state(ca, actual_state);
  }

  is_new_cycle = flags_test(graph, actual_state, STATE_IS_BEING_VISITED);

  if (is_new_cycle)
  {
    UINT cycle_len = 0;

    while (flags_test(graph, actual_state, STATE_IS_BEING_VISITED))
    {
      flags_unset(graph, actual_state, STATE_IS_BEING_VISITED);

      actual_state = next_state(ca, actual_state);

      cycle_len ++;
    }

    /* printf("len %u seed %u start %u ", cycle_len, actual_state, starting_state); */
    printf("%" UFM  "\n", cycle_len);
    fflush(stdout);
  }

  for  (actual_state = starting_state;
        flags_test(graph, actual_state, STATE_IS_BEING_VISITED);
        actual_state = next_state(ca, actual_state))
  {
     flags_unset(graph, actual_state, STATE_IS_BEING_VISITED);
  }
}

int
main(int argc, char *argv[])
{
  UINT i;

  unsigned int N;
  UINT NSTATES;

  unsigned int rule;
  CA *ca;

  char *graph;

  if (argc != 3)
  {
    fprintf(stderr, "usage: %s rule ca-size\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }

  sscanf(argv[1], "%u", &rule);

  sscanf(argv[2], "%u", &N);

  if (DEBUG)
    fprintf(stderr, "rule %u\nN=%u\n", rule, N);

  ca = ca_create(N, rule);

  NSTATES =  2;
  NSTATES <<= (N - 1);

  graph = create_graph(NSTATES);

  for (i = 0; i < NSTATES; ++i)
    flags_unset(graph, i, STATE_ALL_SET_MASK);

  for (i = 0; i < NSTATES; ++i)
    if (!flags_test(graph, i, STATE_HAS_BEEN_VISITED))
      search_cycle(ca, graph, i);

  free_graph(graph);
  ca_destroy(ca);

  return EXIT_SUCCESS;
}
