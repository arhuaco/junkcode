#!/usr/bin/python

import sys

# Now we modify this function to tell us how big the
# sequence is.

def threeplus1(N):
  while N != 1:
    print N,
    if N % 2 != 0: # even number
      N = N * 3 + 1
    else:
      N = N >> 1
threeplus1(int(sys.stdin.readline().strip()))

