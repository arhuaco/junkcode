/*
 * Authors:
 *    Nelson Castillo   nelson@emqbit.com
 *    Andres Calderon   andres.calderon@emqbit.com
 *    Copyright (c) EmQbit ltda <www.emqbit.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *-----------------------------------------------------------------------------
 *
 * History:
 *
 * 2007.04.16 - Fixes for the FLOPSs calculation method
 *              (thanks to Søren Andersen <san@rosetechnology.dk>).
 * 2007.03.14 - Initial release.
 */

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "common.h"
#include "distance.h"

#define MFLOPS 3

typedef struct
{
  char *desc;
  float (*f) (float*, float*, int);
  int   (*flops) (int);
} test_case;

typedef unsigned long long timestamp_t;

static timestamp_t
get_timestamp ()
{
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

int
main(int argc, char *argv[])
{
  test_case tests[] =
    {
      {"Dot with C code        ",  dot_c,           flops_dot},
#if SSE
      {"Dot with SSE x 4       ",  dot_sse,         flops_dot},
      {"Dot with SSE x 16      ",  dot_sse_16,      flops_dot},
#endif
      {"Distance with C code   ",  distance_c ,     flops_distance},
#if SSE
      {"Distance with SSE x 4  ",  distance_sse,    flops_distance},
      {"Distance with SSE x 16 ",  distance_sse_16, flops_distance},
#endif
      {NULL,                       NULL,            NULL}
    };

  float  norm;
  float  secs;
  int i;
  int n;

  float *results;
  float *vector1;
  float *vector2;
  int size = 32;
  int inc = 4;
  int nTimes;
  int nTests = sizeof(tests) /sizeof(test_case) - 1;

  norm = (float)sysconf(_SC_CLK_TCK);

  vector1 = new_vector(1<<MAXPOW2);
  vector2 = new_vector(1<<MAXPOW2);

  results = malloc(nTests * (MAXPOW2 - MINPOW2)*sizeof(float));

  for(size=(1<<MINPOW2),n=0;  size<(1<<MAXPOW2);size=size<<1,n++ )
  {
    inc++;

    for(i=0; tests[i].desc; ++i)
    {
      float dot;
      int j;
      timestamp_t t0, t1;


      nTimes = (int)ceil( (MFLOPS*1e6F)/(float)tests[i].flops(size) );

      t0 = get_timestamp();

      for(j=0; j<nTimes; ++j)
        dot = tests[i].f(vector1, vector2, size);

      t1 = get_timestamp();


      secs = (t1 - t0) / 1000000.0L;

      fprintf(stderr,"nTimes=%d  %d: %s => (flops %f : time:%g us)\n", nTimes, size, tests[i].desc, (tests[i].flops(size)*nTimes)/secs/1e6F, secs);
      results[(n*nTests)+i]=(tests[i].flops(size)*nTimes)/secs/1e6F;
    }
  }

  for(n=0; n<(MAXPOW2 - MINPOW2); ++n)
  {
    printf("%d,\t", 1<<(MINPOW2+n));
    for(i=0; i<nTests; ++i)
    {
      printf("%f,\t", results[(n*nTests)+i]);
    }
   printf("\n");
 }

  free(vector1);
  free(vector2);
  free(results);

  return 0;
}
