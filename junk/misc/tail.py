#!/usr/bin/python

import sys

# tail in python. Use like this: ./tail N file

def tailf(filename, N):

  lines = []
  f = open(filename)
  l = f.readline()
  last = 0

  while len(l) > 0:

    if len(lines) < N:
      lines.append(l)
    else:
      lines[last] = l

    l = f.readline()
    last = (last + 1) % N

  if len(lines) < N:
    for i in lines:
      print i,
  else:
    for i in xrange(last, N):
      print lines[i],
    for i in xrange(0, last):
      print lines[i],

tailf(sys.argv[2], int(sys.argv[1]))
