#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
  int d;
  struct Node *next;
} Node;

Node *
node_new()
{
  Node *n;

  n = malloc(sizeof(Node));

  if (!n)
  {
    fprintf(stderr, "Out of memory in node_new\n");
    exit(EXIT_FAILURE);
  }

  return n;
}

void
node_free(Node *n)
{
  free(n);
}

void
free_list(Node *l)
{
  while (l)
  {
    Node *tmp = l;
    l = l->next;
    node_free(tmp);
  }
}

int list_size (Node *l)
{
  int len = 0;

  while (l)
  {
    len ++;
    l = l->next;
  }

  return len;
}

Node *
reverse_list(Node *l)
{
  Node *rev = NULL;

  while (l)
  {
    Node *tmp = l;
    l = l->next;

    tmp->next = rev;
    rev = tmp;
  }

  return rev;
}

void
print_list(Node *l)
{
  printf ("L -> ");

  while (l)
  {
    printf(" %d -> ", l->d);
    l = l->next;
  }

  printf ("NULL\n");
}

Node *
list_add (Node *l1, Node *l2)
{
  Node *sum;

  if (!l1)
    return l2;
  if (!l2)
    return l1;

  sum = l1;

  while (l1->next)
    l1 = l1->next;

  l1->next = l2;

  return sum;
}


Node *
make_random_list (int size)
{
  Node *L = NULL;

  while (size)
  {
    Node *tmp;

    tmp = node_new();
    tmp->d = (int) (100.0 * (rand() / (RAND_MAX + 1.0))); /* from 0 to 99 */
    tmp->next = L;
    L = tmp;

    size --;
  }

  return L;
}


Node *
merge_sort(Node *list, int size)
{
  int N1, N2, count;
  Node *l1, *l2, *last, *tmp;
  Node head;

  if (size <= 1)
    return list;

  N1 = size / 2;
  N2 = (size + 1) / 2;

  /* make a sublist with N1 elements, take the nodes from the original list */

  l1 = list;
  tmp = list;

  for (count = N1 - 1; count; --count)
    tmp = tmp->next;

  l2 = tmp->next;
  tmp->next = NULL;

  l1 = merge_sort(l1, N1);
  l2 = merge_sort(l2, N2);

  last = &head;

  while (l1 && l2)
  {
    if (l1->d < l2->d)
    {
      tmp = l1;
      l1 = l1->next;
    }
    else
    {
      tmp = l2;
      l2 = l2->next;
    }

    last->next = tmp;
    last = tmp;
  }

  if (l1)
    last->next = l1;
  else if (l2)
    last->next = l2;
  else
    last->next = NULL; /* not reachable */

  return head.next;
}

int
main(int argc, char *argv[])
{
  Node *l1, *l2, *sum;

  l1 = make_random_list(4);
  l2 = make_random_list(5);

  sum = list_add(l1, l2);
  print_list(sum);

  sum = merge_sort(sum, list_size(sum));

  print_list(sum);

  return EXIT_SUCCESS;
}

/* http://www.concentric.net/~Ttwang/tech/inthash.htm */
int
hash32shift(int key)
{
  key = ~key + (key << 15); // key = (key << 15) - key - 1;
  key = key ^ (key >>> 12);
  key = key + (key << 2);
  key = key ^ (key >>> 4);
  key = key * 2057; // key = (key + (key << 3)) + (key << 11);
  key = key ^ (key >>> 16);
  return key;
}
