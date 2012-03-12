#!/usr/bin/python

import random

def qsort(l):
  if len(l) <= 1:
    return  l
  rnd   = random.randint(0, len(l) - 1)
  l[0], l[rnd] = l[rnd], l[0]
  pivot = l[0]
  before = [i for i in l[1:] if  i <= pivot] 
  after  = [i for i in l[1:] if  i > pivot]
  return qsort(before) + [pivot] + qsort(after)

print qsort([10,9,8,7,6,5,4,3,2,1,0, -1])
