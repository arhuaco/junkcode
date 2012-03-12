#include <stdio.h>
#include <stdlib.h>

/*
 * In this program we print all permutations. The algorithm is described
 * in this article.
 *
 * http://msdn.microsoft.com/msdnmag/issues/06/12/TestRun/
 *
 * I think this version is an improvement. You shouldn't
 * use strings as they do in the algorith. It is better to use an index.
 */

typedef struct Perm
{
  int *set;
  int N;
} Perm;

Perm *
perm_new (int N)
{
  Perm *p;
  int i;

  if (N < 2)
  {
    fprintf(stderr, "We need at least two elements\n");
    exit(1);
  }

  p = malloc(sizeof(Perm));
  if (!p)
  {
    fprintf(stderr, "error in perm_new\n");
    exit(1);
  }

  p->set = malloc(sizeof(int) * N);

  if (!p->set)
  {
    fprintf(stderr, "error in perm_new\n");
    exit(1);
  }

  p->N = N;

  for (i = 0; i < N; ++i)
    p->set[i] = i;

  return p;
}

void
perm_free(Perm *p)
{
  free(p->set);
  free(p);
}

/* returns 1 if we could find the next permutation */
int
perm_next (Perm *p)
{
  int i, j, tmp;

  i = p->N - 2;
  while (i > 0 && p->set[i] > p->set[i + 1])
    i--;

  j = p->N - 1;
  while (j > 0 && p->set[i] > p->set[j])
    j--;

  if (i == j)
    return 0;

  tmp = p->set[i];
  p->set[i] = p->set[j];
  p->set[j] = tmp;

  i++;
  j = p->N - 1;

  while (i < j)
  {
    tmp = p->set[i];
    p->set[i++] = p->set[j];
    p->set[j--] = tmp;
  }

  return 1;
}

void
perm_print(Perm *p)
{
  int i;

  printf(" { ");

  for (i = 0; i < p->N; ++i)
    printf("%d ", p->set[i]);

  printf("}\n");
}

int
main (int argc, char *argv[])
{
  Perm *p;
  int i;

  p = perm_new(4);

  do
    perm_print(p);
  while(perm_next(p));

  perm_free(p);

  return 0;
}
