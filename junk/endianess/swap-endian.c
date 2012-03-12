#include <stdio.h>
#include <stdlib.h>
#include <values.h>


unsigned long get_long(unsigned long nLongNumber)
{
   union u {unsigned long vi; unsigned char c[sizeof(unsigned long)];}; 
   union v {unsigned long ni; unsigned char d[sizeof(unsigned long)]; float x;};
   union u un; 
   union v vn; 

   un.vi = nLongNumber; 
   vn.d[0]=un.c[3]; 
   vn.d[1]=un.c[2]; 
   vn.d[2]=un.c[1]; 
   vn.d[3]=un.c[0]; 

   return vn.ni; 
}

int main(int argc, char*argv[])
{
  FILE *p, *q;

  if (argc != 3)
  {
    fprintf(stderr, "Use : %s file-in file-out\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  p = fopen(argv[1], "r");

  if (!p)
  {
    fprintf(stderr, "Couldn't open '%s' for reading\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  q = fopen(argv[2], "w");

  if (!q)
  {
    fprintf(stderr, "Couldn't open '%s' for writing\n", argv[2]);
    exit(EXIT_FAILURE);
  }

  while (!feof(p))
  {
    long f;

    if (fread (&f, sizeof(f), 1, p) == 1)
    {
        f = get_long(f);

        if (fwrite(&f, sizeof(f), 1, q) != 1)
        {
          perror("fwrite");
          exit(EXIT_FAILURE);
        }
    }
    else
      break;
  }

  fclose(p);
  fclose(q);

  return EXIT_SUCCESS;
}
