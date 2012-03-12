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

#ifndef S_FFT_H_
#define S_FFT_H_

#define pi_2 1.57079632679489661923F

#define abs2(v)  (v.r*v.r + v.i*v.i)

#define angle(v) atan2f(v.i,v.r)

#define cmult(c,a,b) c.r=a.r*b.r - a.i*b.i, \
                     c.i=a.r*b.i + a.i*b.r
 
#define csub(c,a,b)  c.r=a.r - b.r, \
                     c.i=a.i - b.i

#define cadd(c,a,b)  c.r=a.r + b.r, \
                     c.i=a.i + b.i


typedef struct {
    float r;
    float i;
}complex;


void fft_init();

void fft_end();

int fft_exec(int N, complex* in);

int  flops_fft(int n);


#endif

