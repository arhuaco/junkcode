#!/usr/bin/python

def fact(x):
  if x <= 1:
    return 1
  return x * fact (x - 1)

def C(n, r):
  return fact(n) / (fact(r) * fact(n-r))

print C(28, 14)
