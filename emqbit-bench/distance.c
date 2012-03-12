#include <math.h>
#include <stdlib.h>

#include "common.h"
#include "distance.h"


int  
flops_dot(int n)
{
  return 2*n;
}

int  
flops_distance(int n)
{
  return 3*n + 1;
}


float
dot_c(float *v1, float *v2, int N)
{
  int i,j;
  float dot = 0.0;

  for(i=0; i<(N>>MINPOW2); ++i)
   for(j=0; j<(MINPOW2<<1); ++j)
   {
     dot+= *(v1++) * *(v2++);
   }

  return dot;
}

float
distance_c(float *v1, float *v2, int N)
{
  int i,j;
  float dist2 = 0.0;

  for(i=0; i<(N>>MINPOW2); ++i)
   for(j=0; j<(MINPOW2<<1); ++j)
   {
     double diff;

    diff = *(v1++) - *(v2++);

    dist2 += diff * diff;
   }

  return sqrt(dist2);
}

#if SSE
float
distance_sse(float *v1, float *v2, int N)
{
  int i;
  float dist2 = 0.0;

  __asm__  __volatile__
  (
   "xorps %%xmm0, %%xmm0 \n"
   :
   :
   : "memory"
  );

  for(i=0; i<N/4; ++i)
  {
    __asm__  __volatile__
    (
    "movups  (%%eax),    %%xmm1                 \n"
    "movups  (%%ebx),    %%xmm2                 \n"
    "subps    %%xmm2,    %%xmm1                 \n"
    "mulps    %%xmm1,    %%xmm1                 \n"
    "addps    %%xmm1,    %%xmm0                 \n"
    :
    : "a"(v1), "b"(v2)
    : "memory"
    );
    
    v1 += 4;
    v2 += 4;
  }
  __asm__  __volatile__
  (
   "movss       %%xmm0,  %%xmm4            \n" // xmm4[0] := xmm0[0]
   "shufps      $0b11100101, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[1]
   "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
   "shufps      $0b11100110, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[2]
   "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
   "shufps      $0b11100111, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[3]
   "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
   "movss       %%xmm4, (%%edx)            \n" // *pi := xmm4[0]
   :
   : "d"(&dist2)
   : "memory"
  );

  for(i=0; i<N%4; ++i)
  {
    double diff;

    diff = *(v1++) - *(v2++);

    dist2 += diff * diff;
  }

  return sqrt(dist2);
}

float
distance_sse_16(float *v1, float *v2, int N)
{
  int i;
  int SSEN;
  float dist2 = 0.0;

  float *v3;
  float *v4;

  __asm__  __volatile__
  (
   "xorps %%xmm0, %%xmm0 \n"
   :
   :
   : "memory"
  );

  v3 = v1 + 4;
  v4 = v2 + 4;
 
  SSEN = N / 16;
  
  for(i=0; i<SSEN; ++i)
  {

    __asm__  __volatile__
    (
    "movups  (%%eax),    %%xmm1                 \n"
    "movups  (%%ecx),    %%xmm3                 \n"
    "movups  (%%ebx),    %%xmm2                 \n"
    "movups  (%%edx),    %%xmm4                 \n"
    
    "subps    %%xmm2,    %%xmm1                 \n"
    "subps    %%xmm4,    %%xmm3                 \n"
    
    "mulps    %%xmm1,    %%xmm1                 \n"
    "mulps    %%xmm3,    %%xmm3                 \n"
    
    "addps    %%xmm1,    %%xmm0                 \n"
    "addps    %%xmm3,    %%xmm0                 \n"
    :
    : "a"(v1), "b"(v2), "c"(v3), "d"(v4)
    : "memory"
    );
    
    v1 += 8;
    v2 += 8;
    v3 += 8;
    v4 += 8;

    __asm__  __volatile__
    (
    "movups  (%%eax),    %%xmm1                 \n"
    "movups  (%%ecx),    %%xmm3                 \n"
    "movups  (%%ebx),    %%xmm2                 \n"
    "movups  (%%edx),    %%xmm4                 \n"

    "subps    %%xmm2,    %%xmm1                 \n"
    "subps    %%xmm4,    %%xmm3                 \n"

    "mulps    %%xmm1,    %%xmm1                 \n"
    "mulps    %%xmm3,    %%xmm3                 \n"

    "addps    %%xmm1,    %%xmm0                 \n"
    "addps    %%xmm3,    %%xmm0                 \n"
    :
    : "a"(v1), "b"(v2), "c"(v3), "d"(v4)
    : "memory"
    );

    v1 += 8;
    v2 += 8;
    v3 += 8;
    v4 += 8;
  }

  N -= SSEN * 16;
  SSEN = N / 4;

  for(i=0; i<SSEN; ++i)
  {
    __asm__  __volatile__
    (
      "movups  (%%eax),    %%xmm1                 \n"
      "movups  (%%ebx),    %%xmm2                 \n"
      "subps    %%xmm2,    %%xmm1                 \n"
      "mulps    %%xmm1,    %%xmm1                 \n"
      "addps    %%xmm1,    %%xmm0                 \n"
     :
     : "a"(v1), "b"(v2)
     : "memory"
    );
    
    v1 += 4;
    v2 += 4;
  }
 
  N -= SSEN * 4;
  
  __asm__  __volatile__
  (
   "movss       %%xmm0,  %%xmm4            \n" // xmm4[0] := xmm0[0]
   "shufps      $0b11100101, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[1]
   "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
   "shufps      $0b11100110, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[2]
   "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
   "shufps      $0b11100111, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[3]
   "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
   "movss       %%xmm4, (%%edx)            \n" // *pi := xmm4[0]
   :
   : "d"(&dist2)
   : "memory"
  );

  for(i=0; i < N; ++i)
  {
    double diff;

    diff = *(v1++) - *(v2++);

    dist2 += diff * diff;
  }
  
  return sqrt(dist2);
}

float
dot_sse_16(float *v1, float *v2, int N)
{
  int i;
  int SSEN;
  float dot = 0.0;
  float *v3, *v4;

  __asm__  __volatile__
  (
    "xorps %%xmm0, %%xmm0 \n"
    :
    : "a"(v1), "b"(v2)
    : "memory"
  );

  SSEN = N/16;

  v3 = v1 + 4;
  v4 = v2 + 4;
  
  for(i=0; i<SSEN; ++i)
  {
        
    __asm__  __volatile__
    (
      "movups  (%%eax),  %%xmm1         \n"
      "movups  (%%ecx),  %%xmm3         \n"

      "movups  (%%ebx),  %%xmm2         \n"
      "movups  (%%edx),  %%xmm4         \n"

      "mulps    %%xmm2,    %%xmm1     \n"
      "addps    %%xmm1,    %%xmm0     \n"
      
      "mulps    %%xmm4,    %%xmm3     \n"
      "addps    %%xmm3,    %%xmm0     \n"
            
     :
     : "a"(v1), "b"(v2), "c"(v3), "d"(v4)
     : "memory"
    );

    v1 += 8;
    v2 += 8;
    v3 += 8;
    v4 += 8;

    __asm__  __volatile__
    (
      "movups  (%%eax),  %%xmm1         \n"
      "movups  (%%ecx),  %%xmm3         \n"
      "movups  (%%ebx),  %%xmm2         \n"
      "movups  (%%edx),  %%xmm4         \n"
      "mulps    %%xmm2,    %%xmm1     \n"
      "addps    %%xmm1,    %%xmm0     \n"
      "mulps    %%xmm4,    %%xmm3     \n"
      "addps    %%xmm3,    %%xmm0     \n"
     :
     : "a"(v1), "b"(v2), "c"(v3), "d"(v4)
     : "memory"
    );

    v1 += 8;
    v2 += 8;
    v3 += 8;
    v4 += 8;
  }
   
  N = N - SSEN * 16;
 
  SSEN = N / 4;

  for(i=0; i<SSEN; ++i)
  {
    __asm__  __volatile__
    (
      "movups  (%%eax),    %%xmm1                 \n"
      "movups  (%%ebx),    %%xmm2                 \n"
      "mulps    %%xmm2,    %%xmm1                 \n"
      "addps    %%xmm1,    %%xmm0                 \n"
     :
     : "a"(v1), "b"(v2)
     : "memory"
    );

    v1+=4;
    v2+=4;
  }

  N -= SSEN * 4;
  
  __asm__  __volatile__
  (
    "movss       %%xmm0,  %%xmm4            \n" // xmm4[0] := xmm0[0]
    "shufps      $0b11100101, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[1]
    "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
    "shufps      $0b11100110, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[2]
    "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
    "shufps      $0b11100111, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[3]
    "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
    "movss       %%xmm4, (%%edx)            \n" // *pi := xmm4[0]
    :
    : "d"(&dot)
    : "memory"
  );

  for(i=0; i<N%4; ++i)
  {
   dot+= *(v1++) * *(v2++);
  }

  return dot;
}
                                                                    

float
dot_sse(float *v1, float *v2, int N)
{
  int i;
  int SSEN;
  float dot = 0.0;


  __asm__  __volatile__
  (
   "xorps %%xmm0, %%xmm0 \n"
   :
   :
   : "memory"
  );

  SSEN = N/4;

  for(i=0; i<SSEN; ++i)
  {
    __asm__  __volatile__
    (
      "movups  (%%eax),    %%xmm1                 \n"
      "movups  (%%ebx),    %%xmm2                 \n"
      "mulps    %%xmm2,    %%xmm1                 \n"
      "addps    %%xmm1,    %%xmm0                 \n"

      :
      : "a"(v1), "b"(v2)
      : "memory"
    );

    v1+=4;
    v2+=4;
  }

  __asm__  __volatile__
  (
    "movss       %%xmm0,  %%xmm4            \n" // xmm4[0] := xmm0[0]
    "shufps      $0b11100101, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[1]
    "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
    "shufps      $0b11100110, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[2]
    "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
    "shufps      $0b11100111, %%xmm0, %%xmm0\n" // xmm0[0] := xmm0[3]
    "addss       %%xmm0,  %%xmm4            \n" // xmm4[0] += xmm0[0]
    "movss       %%xmm4, (%%edx)            \n" // *pi := xmm4[0]
    :
    : "d"(&dot)
    : "memory"
  );

  for(i=0; i<N%4; ++i)
  {
   dot+= *(v1++) * *(v2++);
  }
  
  return dot;
}

#endif

float *
new_vector(int size)
{
  int i;
 
  float *new;
  
  new = malloc(sizeof(float) * size);

  for(i = 0; i < size; ++i)
    new[i] = (float)rand()/(float)RAND_MAX - 0.5;

  return new;
}
