#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <neural-unit.h>

struct NeuralUnit
{
  double mem;                   /* Memory of the previous time-step */
  double sum;                   /* Accumulator */
  double persistent_input;      /* Input that will always be present */
};


NeuralUnit *
neural_unit_new ()
{
  NeuralUnit *new;

  new = malloc (sizeof (NeuralUnit));

  if (!new)
    {
      fprintf (stderr, "neural_unit_new : out of memory.");
      exit (1);
    }

  return new;
}

void
neural_unit_destroy (NeuralUnit * u)
{
  assert (u);
  free (u);
}

void
neural_unit_init (NeuralUnit * u, double initial_state)
{
  assert (u);
  assert (initial_state >= -1.0 && initial_state <= 1.0);

  u->mem = initial_state;
  u->sum = 0;
  u->persistent_input = 0;
}

void
neural_unit_input_add (NeuralUnit * u, double value)
{
  assert (u);
  u->sum += value;
}

static double
sigma (double v)
{
  return 2.0 / (1.0 + exp (-v)) - 1;
}

void
neural_unit_iterate (NeuralUnit * u)
{
  assert (u);

  u->mem = sigma (u->sum + u->persistent_input);
  u->sum = 0;
}

void
neural_unit_signal (const NeuralUnit * s, NeuralUnit * d, double weight)
{
  assert (s);
  assert (d);

  d->sum += s->mem * weight;
}

void
neural_unit_persistent_input_set (NeuralUnit * u, double value)
{
  assert (u);
  assert (value >= -1.0);
  assert (value <= 1.0);

  u->persistent_input = value;
}

void
nural_unit_input_add (NeuralUnit * u, double value)
{
  assert (u);
  assert (value >= -1.0);
  assert (value <= 1.0);

  u->sum += value;
}

double
neural_unit_output_get (NeuralUnit * u)
{
  assert (u);

  return u->mem;
}
