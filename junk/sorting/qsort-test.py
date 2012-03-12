#!/usr/bin/python

import time
import random
import sys

# The partition algorithm is described here:
# http://ocw.mit.edu/NR/rdonlyres/Electrical-Engineering-and-Computer-Science/6-046JFall-2005/9427EAAA-9522-4B26-8FDA-99344B8518AD/0/lec4.pdf

def generic_partition(list, start, end, random_pivot):

  if random_pivot:
    i = random.randint(start, end) # randomize the pivot
    tmp = list[start]
    list[start] = list[i]
    list[i] = tmp

  pivot = list[start]
  i = start

  for j in xrange(start + 1, end + 1):
    if list[j] <= pivot:
      i = i + 1
      tmp = list[i]
      list[i] = list[j]
      list[j] = tmp

  list[start] = list[i]
  list[i] = pivot
  return i

def normal_partition(list, start, end):
  return generic_partition(list, start, end, False)

def random_partition(list, start, end):
  return generic_partition(list, start, end, True)

def qsort_aux(v, start, end, partition_func):
  if start < end:
    p = partition_func(v, start, end)
    qsort_aux(v, start, p - 1, partition_func)
    qsort_aux(v, p + 1, end, partition_func)

def qsort(v, partition_func):
  qsort_aux(v, 0, len(v) - 1, partition_func)

def do_test(set, size, partition_func, txt):
  t = time.time()
  qsort(set, partition_func)
  t = time.time() - t
  print ("%s (%s secs)" % (txt, t))

sys.setrecursionlimit(30000)

for N in [100, 1000, 2000, 5000, 10000, 20000]:
  ord_set = range(N)
  ran_set = [random.randint(0, 10000) for i in xrange(N)]

  print "Elements : %s" % N

  do_test(ord_set * 1, N, normal_partition, "ordered set, normal partition")
  do_test(ord_set * 1, N, random_partition, "ordered set, random pivot")
  do_test(ran_set * 1, N, normal_partition, "random set, normal partition")
  do_test(ran_set * 1, N, random_partition, "random set, random pivot")
