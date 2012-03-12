#include <stdio.h>
#include <stdlib.h>
#include <values.h>


float get_float(float *f)
{
   unsigned long nLongNumber = *((unsigned long*)f);

   union u {unsigned long vi; unsigned char c[sizeof(unsigned long)];}; 
   union v {float          x; unsigned char d[sizeof(unsigned long)];};
   union u un; 
   union v vn; 

   un.vi = nLongNumber; 
   vn.d[0]=un.c[3]; 
   vn.d[1]=un.c[2]; 
   vn.d[2]=un.c[1]; 
   vn.d[3]=un.c[0]; 

   return vn.x; 
}

int main(int argc, char*argv[])
{
  FILE *p;
  float max = MINFLOAT , min = MAXFLOAT;
  double sum = 0.0;
  unsigned int count = 0;
  int bigendian;

  if (sizeof(long) != sizeof(float))
  {
    fprintf(stderr, "sizeof(long) != sizeof(float)\n");
    exit(EXIT_FAILURE);
  }

  if (argc != 3)
  {
    fprintf(stderr, "Use : %s filename bigendian(1/0)\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  p = fopen(argv[1], "r");
  bigendian = atoi(argv[2]);

  if (!p)
  {
    fprintf(stderr, "Couldn't open '%s' for reading", argv[1]);
    exit(EXIT_FAILURE);
  }

  while (!feof(p))
  {
    float f;

    if (fread (&f, sizeof(f), 1, p) == 1)
    {
      if (bigendian)
        f = get_float (&f);

      if (f > max)
        max = f;

      if (f < min)
        min = f;

      count ++;
      sum += f;
    }
  }

  fclose(p);

  if (count)
    printf("max:%g min:%g avg:%g\n", max, min, sum / (double)count);
  else
    printf("No data in file\n");

  return EXIT_SUCCESS;
}
