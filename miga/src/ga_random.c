#include <ga_random.h>

/*
 * RAN2. See "numerical recipes in C".
 */

#define IM1 2147483563L
#define IM2 2147483399L
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014L
#define IA2 40692L
#define IQ1 53668L
#define IQ2 52774L
#define IR1 12211L
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

static long idum2 = 123456789;
static long iy = 0;
static long iv[NTAB];
static long idum = 0;

static void
sran2 (long seed)
{
  int j;
  long k;

  idum = seed;
  if (idum == 0)
    idum = 1;
  if (idum < 0)
    idum = -idum;
  idum2 = (idum);
  for (j = NTAB + 7; j >= 0; j--)
    {
      k = (idum) / IQ1;
      idum = IA1 * (idum - k * IQ1) - k * IR1;
      if (idum < 0)
	idum += IM1;
      if (j < NTAB)
	iv[j] = idum;
    }
  iy = iv[0];
}

static float
ran2 ()
{
  int j;
  long k;
  float temp;

  k = (idum) / IQ1;
  idum = IA1 * (idum - k * IQ1) - k * IR1;
  if (idum < 0)
    idum += IM1;
  k = idum2 / IQ2;
  idum2 = IA2 * (idum2 - k * IQ2) - k * IR2;
  if (idum2 < 0)
    idum2 += IM2;
  j = iy / NDIV;
  iy = iv[j] - idum2;
  iv[j] = idum;
  if (iy < 1)
    iy += IMM1;
  if ((temp = AM * iy) > RNMX)
    return RNMX;
  else
    return temp;
}

void
random_seed_set(long seed)
{
  sran2 (seed);
}

float
random_float_get()
{
  return ran2();
}

int
random_int_get(int max_plus_1)
{
  return ran2() * max_plus_1;
}
