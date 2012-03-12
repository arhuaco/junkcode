#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* A silly program that converts a binary 32-bit number to unsigned int */

#define PROGRAM_NAME "bin2dec"

int
main (int argc, char *argv[])
{
  char *digits;
  unsigned int number = 0, add = 1;
  int i;
  
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s binary_number\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }

  digits = argv[1];

  if (strlen(digits) != 32)
  {
    fprintf(stderr, "%s binary_number should be 32 digits long\n", PROGRAM_NAME);
    exit(EXIT_FAILURE);
  }

  for (i = 31; i >= 0 ; --i, add *= 2)
  {
    if (digits[i] == '1')
    {
      number += add;
    }
    else if(digits[i] != '0')
    {
      fprintf(stderr, "%s: invalid digit '%c' (%d)\n", PROGRAM_NAME,
                      digits[i], digits[i]);
      exit(EXIT_FAILURE);
    }
  }

  printf("%u", number);
  
  return EXIT_SUCCESS;
}
