#include <neural-unit.h>
#include <neural-net.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct NeuralNet
{
  NeuralUnit **net;
  int size;
};


NeuralNet *
neural_net_new (int size)
{
  NeuralUnit **units;
  NeuralNet *new;
  int i;

  assert (size > 0);

  units = malloc (sizeof (NeuralUnit *) * size);

  if (!units)
    {
      fprintf (stderr, "neural_net_new: out of memory");
      exit (1);
    }

  for (i = 0; i < size; ++i)
    units[i] = neural_unit_new ();

  new = malloc (sizeof (NeuralNet));

  if (!new)
    {
      fprintf (stderr, "neural_net_new: out of memory");
      exit (1);
    }

  new->size = size;
  new->net = units;

  return new;
}

void
neural_net_destroy (NeuralNet * n)
{
  int i;

  assert (n);
  assert (n->net);
  assert (n->size > 0);

  for (i = 0; i < n->size; i++)
    {
      neural_unit_destroy (n->net[i]);
    }

  free (n);
}

void
neural_net_init (NeuralNet * n, double initial_state)
{
  int i;

  assert (n);
  assert (n->net);
  assert (n->size > 0);

  for (i = 0; i < n->size; i++)
    neural_unit_init (n->net[i], initial_state);
}

void
neural_net_iterate (NeuralNet * n, double *weights)
{
  int i, j;

  assert (n);
  assert (n->net);
  assert (n->size > 0);

  for (i = 0; i < n->size; i++)
    for (j = 0; j < n->size; j++)
      neural_unit_signal (n->net[i], n->net[j], weights[i * n->size + j]);

  for (i = 0; i < n->size; i++)
    neural_unit_iterate (n->net[i]);
}

void
neural_net_input_send (NeuralNet * n, int who, double value)
{
  assert (n);
  assert (n->net);
  assert (n->size > 0);
  assert (who < n->size && who >= 0);

  neural_unit_input_add (n->net[who], value);
}

void
neural_net_persistent_input_set (NeuralNet * n, int who, double value)
{
  assert (n);
  assert (n->net);
  assert (n->size > 0);
  assert (who < n->size && who >= 0);

  neural_unit_persistent_input_set (n->net[who], value);
}

double
neural_net_output_get (NeuralNet * n, int who)
{
  assert (n);
  assert (n->net);
  assert (n->size > 0);
  assert (who < n->size);

  return neural_unit_output_get (n->net[who]);
}
