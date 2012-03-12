#!/usr/bin/python

import sys

# Now we modify this function to tell us how big the
# sequence is.

def threeplus1(N):
  count = 0
  while N != 1:
    if N % 2 != 0: # even number
      N = N * 3 + 1
    else:
      N = N / 2
    count += 1
  return count

# And Now we test numbers from START to END
print 'Let\'s look for long sequences'
print 'type the starting number (example 1) then ENTER -> ',
START= int(sys.stdin.readline().strip())
print 'type the ending number (example 1000) then ENTER -> ',
END= int(sys.stdin.readline().strip())
x = []
for i in range (START, END):
  x.append((threeplus1(i), i))
x.sort()
x.reverse()
print x[:10]
