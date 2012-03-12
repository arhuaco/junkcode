/*
 * Authors:
 *    Jorge Victorino
 *    Andres Calderon   andres.calderon@emqbit.com
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include "cfft.h"
#include "common.h"

#define MFLOPS 2

complex *tableW;
int *bndx;
int *ndx;

typedef unsigned long long timestamp_t;

static timestamp_t
get_timestamp ()
{
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}


int
fft_index (int i)
{
  return bndx[i];
}

void
fft_init (int N)
{
  tableW = malloc ((N / 2) * sizeof (complex));

  bndx = malloc (N * sizeof (int));
  ndx = malloc ((N / 2) * sizeof (int));


  int i, j;
  ndx[0] = 0;
  for (i = 1; i < N / 2; i = i * 2)
    for (j = 0; j < i; j++)
      {
        ndx[j] *= 2;
        ndx[j + i] = ndx[j] + 1;
      }

  bndx[0] = 0;
  for (i = 1; i < N; i = i * 2)
    for (j = 0; j < i; j++)
      {
        bndx[j] *= 2;
        bndx[j + i] = bndx[j] + 1;
      }

  for (i = 0; i < N / 2; i++)
    {
      tableW[i].r = cos (ndx[i] * 2.F * M_PI / (float) N);
      tableW[i].i = -sin (ndx[i] * 2.F * M_PI / (float) N);
    }
}

void
fft_end ()
{
  free (tableW);

  free (bndx);

  free (ndx);
}


int
fft_exec (int N, complex * in)
{
  int flops;
  unsigned int n = N;
  unsigned int a, b, i, j, k, r, s;
  complex w, p;

  flops = 0;
  for (i = 1; i < N; i = i * 2)
    {
      n = n >> 1;
      for (k = 0; k < i; k++)
        {
          w = tableW[k];

          r = 2 * n * k;
          s = n * (1 + 2 * k);

          for (j = 0; j < n; j++)
            {
              flops += 10;
              a = j + r;
              b = j + s;
              cmult (p, w, in[b]);      //6 flop
              csub (in[b], in[a], p);   //2 flop
              cadd (in[a], in[a], p);   //2 flop

            }
        }
    }

  return flops;
}

int
flops_fft (int n)
{
  return (5 * (n) * (int) (log ((float) n) / log (2.0F) + 0.5));
}

#if 1
int
main ()
{
  int i;
  int N, n;
  int nTimes;
  float secs;
  float *results = malloc ((MAXPOW2 - MINPOW2) * sizeof (float));
  timestamp_t t0, t1;


  for (N = (1 << MINPOW2), n = 0; N < (1 << MAXPOW2); N = N << 1, n++)
    {
      complex *in = malloc ((N) * sizeof (complex));
      complex *out = malloc ((N) * sizeof (complex));

      for (i = 0; i < N; i++)
        in[i].r = i, in[i].i = 0;

      fft_init (N);

      nTimes = (int) ceil ((MFLOPS * 1e6F) / (float) flops_fft (N));

      t0 = get_timestamp();

      for (i = 0; i < nTimes; i++)
        {
          memcpy (out, in, (N) * sizeof (complex));
          fft_exec (N, out);
        }

      t1 = get_timestamp();

      secs = (t1 - t0) / 1000000.0L;

      free (in);

      free (out);

      fft_end ();

      fprintf (stderr, "nTimes=%d N=%d:  (flops %f : time:%g us)\n", nTimes,
               N, (flops_fft (N) * nTimes) / secs * 1e-6F, secs);
      results[n] = (flops_fft (N) * nTimes) / secs * 1e-6F;
    }
  for (n = 0; n < (MAXPOW2 - MINPOW2); ++n)
    printf ("%d, %f\n", 1 << (MINPOW2 + n), results[n]);

  free (results);
}
#endif
