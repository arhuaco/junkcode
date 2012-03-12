#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void
reverse_string_section(char *s, int start, int end)
{
  while (start < end)
  {
    char tmp;

    tmp = s[start];
    s[start] = s[end];
    s[end] = tmp;
    start ++;
    end --;
  }
}

void
reverse_words (char *s)
{
  int l;
  int i, j;

  l = strlen(s);

  /* reverse the whole string first */
  reverse_string_section(s, 0, l - 1);

  /* find the start and the end of each word */

  i = 0;
  while (s[i])
  {
    while (isspace(s[i])) /* beginning of word */
      i++;

    j = i;
    while(s[j] && !isspace(s[j])) /* rest of the word */
      j++;

    j--;

    if (i < j)
      reverse_string_section(s, i, j);

    i = j + 1;
  }
}

int
main (int argc, char *argv[])
{
  char s[16];

  strcpy(s, "this is a test");
  reverse_words(s);
  puts(s);

  return EXIT_SUCCESS;
}
