#ifndef _DISTANCE_H
#define _DISTANCE_H

#include <math.h>

int    flops_dot(int n);

int    flops_distance(int n);

float  dot_c(float *v1, float *v2, int N);

float  distance_c(float *v1, float *v2, int N);

#if SSE

float  distance_sse(float *v1, float *v2, int N);

float  distance_sse_16(float *v1, float *v2, int N);

float  dot_sse_16(float *v1, float *v2, int N);

float  dot_sse(float *v1, float *v2, int N);

#endif

float* new_vector(int size);

#endif
