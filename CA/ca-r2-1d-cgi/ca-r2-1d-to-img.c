#include <util.h>
#include <caiterate.h>

#include <gd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_WIDTH 100000
#define MAX_HEIGHT 100000

static char *program_name = "ca-r2-1d-to-img";

void getenv_uint (unsigned int *var, char *name);
void getenv_char (char **var, char *name);

int
main (int argc, char *argv[])
{
  CA *ca;  
  
  unsigned int rule = 3133097257u;
  unsigned int width = 512, height = 256;
  int row;
  
  gdImagePtr im;
  FILE *pngout;
  int   pngout_close = 0;
  char *filename;
  int   filename_free = 0;
  int black, white;

  unsigned int seed;
  char *initial;
  unsigned int initial_row;

  seed = time(NULL);
  getenv_uint(&seed, "CA_SEED");
  srand(seed);

  getenv_uint(&rule, "CA_RULE");
  getenv_uint(&width, "CA_WIDTH");
  getenv_uint(&height, "CA_HEIGHT");
  
  if (width > MAX_WIDTH)
    width = MAX_WIDTH;

  if (height > MAX_HEIGHT)
    height = MAX_HEIGHT;

  initial_row = width / 2;

  getenv_uint(&initial_row, "CA_INITROW");

  ca = ca_create(width, rule);
  ca_rule_set(ca, rule);

  if (!(initial = getenv("CA_INITIALCONF"))) /* a string with 1's and 0's */
    ca_init_random(ca);
  else
  {
    char *s = initial;
    unsigned int i;

    for (i = 0; i < width; ++i)
      ca_cell_set(ca, i, 0);

    i = initial_row - strlen(initial) / 2;

    for ( ; i < width && *s; s++, i++)
      if (*s == '1')
        ca_cell_set(ca, i, 1);
  }

  im = gdImageCreate(width, height);
  white = gdImageColorAllocate(im, 255, 255, 255);
  black = gdImageColorAllocate(im, 0, 0, 0);

  for (row = 0; row < height; ++row)
  {
    int col;

    for (col = 0; col < width; col ++)
      if (ca_cell_get(ca, col))
        gdImageSetPixel(im, col, row, black); 

    ca_iterate(ca, 1);
  }
  
  filename = getenv("CA_FILE");
  
  if (filename && !strncmp("stdout", filename, 7))
    pngout = stdout;
  else
  {
    if (!filename)
    {
      int size = 32;
    
      MALLOC(filename, size);
      filename_free = 1;

      snprintf(filename, size, "ca%u.png", rule);
      filename[size - 1] = 0;
    }

    pngout = fopen(filename, "w");
    pngout_close = 1;
  }

  if (pngout)
    gdImagePng(im, pngout);
  else
  {
    fprintf(stderr, "%s: could't open file '%s'\n", program_name, filename);
    exit(EXIT_FAILURE);
  }

  if (pngout_close)
    fclose(pngout);
  
  gdImageDestroy(im);

  if (filename_free)
    free(filename);

  ca_destroy(ca);

  return EXIT_SUCCESS;
}

void
getenv_uint(unsigned int *var, char *name)
{
  char *str;

  ASSERT(var);
  ASSERT(name);

  if ((str = getenv(name)) && *str)
  {
    char *strend;
    unsigned long int num;

    num = strtoul(str, &strend, 10);
    
    if (!(*strend))
      *var = num;
  }
}

void
getenv_char(char **var, char *name)
{
  char *str;

  ASSERT(var);
  ASSERT(name);

  if ((str = getenv(name)) && *str)
    *var = str;
}
