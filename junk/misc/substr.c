#include <stdio.h>

inline int
is_prefix(char *a, char *b)
{
  while (*a && *b)
    if (*(a++) != *(b++))
      return 0;

  if (!(*a))
    return 1;

  return 0;
}

int
is_substring(char *a, char *b)
{
  while (*b)
    if (is_prefix(a, b++))
      return 1;
  return 0;
}

int
main(int argc, char *argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s find string\n", argv[0]);
    return 1;
  }

  if (is_substring(argv[1], argv[2]))
    printf("%s is substring of %s\n", argv[1], argv[2]);
  else
    printf("%s is NOT a substring of %s\n", argv[1], argv[2]);
}
