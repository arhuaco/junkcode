#!/usr/bin/python
# http://wiki.freaks-unidos.net/weblogs/arhuaco/compare-gnome-sort-with-bubble-sort-in-python

import random
import time

def gnome_sort(A):
  i = 0
  comp = 0
  swap = 0
  while i < len(A):
    comp += 1
    if i == 0 or A[i - 1] <= A[i]:
      i += 1
    else:
      A[i], A[i - 1] = A[i - 1], A[i]
      swap += 1
      i -= 1
  return comp, swap

def bubble_sort(A):
  comp = 0
  swap = 0
  for i in xrange(len(A) - 1):
    for j in xrange(i + 1, len(A)):
      comp += 1
      if (A[i] > A[j]):
        swap += 1
        A[i], A[j] = A[j], A[i]
  return comp, swap

def do_test(s, size, algorithm, txt):
  t = time.time()
  c, s = algorithm(s)
  t = time.time() - t
  print ("%s (%.5f secs) %d comparisons %d swaps" % (txt, t, c, s))

for N in [1000, 10000]:
  print "Elements : %s" % N
  Sorted = range(N)
  do_test(Sorted * 1, N, gnome_sort,  'Gnome  - ordered array')
  do_test(Sorted * 1, N, bubble_sort, 'Bubble - ordered array')
  Reversed = range(N)
  Reversed.reverse()
  do_test(Reversed * 1, N, gnome_sort,  'Gnome  - reversed array')
  do_test(Reversed * 1, N, bubble_sort, 'Bubble - reversed array')
  Random = [random.randint(0, 10000) for i in xrange(N)]
  do_test(Random * 1, N, gnome_sort,  'Gnome  - random array')
  do_test(Random * 1, N, bubble_sort, 'Bubble - random array')
