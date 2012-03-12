#include <entropy_meter.h>
#include <util.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef M_LN2 /* logl(2.0) */
  #define M_LN2 0.69314718055994530942
#endif

struct EntropyMeter
{
  /* Subsequence length */
  int      h;

  /* Temporal buffers to track subsequences */
  int     *Counter;  /* temporal buffers to track subsequences */

  /* 2^h subsquences of lenght h */
  int      NSubsequences;

  /* How many bits we measure */
  int      TimeSteps;

  /* Auxiliar vars to track subsequences */
  int      Sum;
  int      CurrentSequence;
  int      j;
};

EntropyMeter *
entropy_meter_new (int h)
{
  EntropyMeter *_new;

  ASSERT(h > 3);

  MALLOC(_new, sizeof(EntropyMeter));

  _new->h = h;

  _new->NSubsequences = pow(2, h);

  MALLOC(_new->Counter, sizeof (int) * _new->NSubsequences);

  entropy_meter_reset(_new);

  return _new;
}

void
entropy_meter_destroy (EntropyMeter * e)
{
  ASSERT(e);
  ASSERT(e->Counter);

  free(e->Counter);
  free(e);
}

void
entropy_meter_reset (EntropyMeter * e)
{
  int i;

  ASSERT(e);
  ASSERT(e->Counter);

  for (i = 0; i < e->NSubsequences; ++i)
  {
    e->Counter[i] = 0;
  }

  e->Sum = 1;
  e->CurrentSequence = 0;
  e->j = 1;
  e->TimeSteps = 0;
}


void
entropy_meter_count (EntropyMeter * e, int value)
{
  ASSERT(value == 0 || value == 1);

  if (value)
  {
    e->CurrentSequence += e->Sum;
  }

  if (e->j >= e->h)
  {
    e->Counter[e->CurrentSequence]++;
    e->Sum = 1;
    e->CurrentSequence = 0;
    e->j = 1;
  }
  else
  {
    e->Sum *= 2;
    e->j++;
  }

  e->TimeSteps++;
}

long double
entropy_meter_get_measure (EntropyMeter * e)
{
  long double Eh = 0.0L;
  long double p;
  int i;

  ASSERT(e->TimeSteps > 0);

  for (i = 0; i < e->NSubsequences; ++i)
  {
    if (e->Counter[i])
    {
      p = ((long double) e->Counter[i]) /
          (((long double) e->TimeSteps) / ((long double) e->h));

      Eh += p * (logl(p) / M_LN2);
    }
  }

  return -Eh;
}

#if 0
long double
entropy_meter_get_measure_min_sq (EntropyMeter * e)
{
  int i;
  int sum = 0;
  int max_each;

  ASSERT(e->TimeSteps > 0);

  max_each = e->NSubsequences * e->h;

  for (i = 0; i < e->NSubsequences; ++i)
  {
    if (e->Counter[i] <= 2 * max_each && e->Counter[i])
    {
      int sq = e->Counter[i] - max_each;

      sum += max_each * max_each - sq * sq;
    }
  }

  return (long double)sum / (long double)(max_each * max_each * e->NSubsequences);
}
#endif
