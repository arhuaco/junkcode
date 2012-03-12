#include <stdio.h>

void
print_sums(int *v, int size, int s)
{
  int i, j;

  i = 0;
  j = size - 1;

  while (i < j)
  {
    int sum = v[i] + v[j];

    if (sum > s)
      j = j - 1;
    else if (sum < s)
      i = i + 1;
    else {
      printf ("%d, %d\n", v[i] , v[j]);
      i = i + 1;
    }
  }

}

int
main(int argc, char *argv[])
{
  int v[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  print_sums(v, 9, 10);
}
