#include <tam_hashfunc.h>
/*
 * This hash function was borrowed from GCC 3.2
 * Copyright (C) 2000, 2001 Free Software Foundation, Inc
 * /usr/src/gcc-3.2/gcc/hash.c
 */

unsigned long string_hash (const unsigned char *s)
{
  unsigned long hash;
  unsigned char c;
  unsigned int len;

  hash = 0;
  len = 0;

  while ((c = *s++) != '\0')
    {
      hash += c + (c << 17);
      hash ^= hash >> 2;
      ++len;
    }

  hash += len + (len << 17);
  hash ^= hash >> 2;

  return hash;
}
