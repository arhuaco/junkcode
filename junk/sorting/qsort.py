#!/usr/bin/python

import random

def partition(list, start, end):

  if end - start > 2:
    i = random.randint(start, end) # randomize the pivot
    list[start], list[i] = list[i], list[start]

  pivot = list[start]
  i = start

  for j in xrange(start + 1, end + 1):
    if list[j] <= pivot:
      i = i + 1
      list[i], list[j] = list[j], list[i]

  list[start] = list[i]
  list[i] = pivot
  return i

def qsort_aux(v, start, end):
  if start < end:
    if end - start == 1:
      if v[start] > v[end]:
        v[start], v[end] = v[end], v[start]
    else:
      p = partition(v, start, end)
      qsort_aux(v, start, p - 1)
      qsort_aux(v, p + 1, end)

def qsort(v):
  qsort_aux(v, 0, len(v) - 1)

random.seed(2007)

if __name__ == "__main__":
  set = range(0, 100)
  set.reverse()

  qsort(set)
  print set
