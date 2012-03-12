#!/usr/bin/python

import sys
import musicgen

N = 27
sequence = []
while N != 1:
  if N % 2: # odd
    N = N *3 + 1
  else:
    N = N / 2
  sequence.append(N)

file = open('out.sw.xml', 'w')
musicgen.write_header(file, sequence)
musicgen.add_instrument(file, "GUITAR_HARMONICS", sequence, "inc-dec", 1)
musicgen.add_instrument(file, "BASS", sequence, "inc-dec", 2)
musicgen.add_instrument(file, "GUITAR", sequence, "map", 1)
musicgen.write_end(file)
