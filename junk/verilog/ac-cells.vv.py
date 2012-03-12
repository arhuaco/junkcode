#!/usr/bin/env python

import sys


dash_loc = sys.argv[0].rfind ('-')

if dash_loc == -1:
  print >> sys.stderr, "No hash in python filename\n"
  sys.exit(1)

filename_father = sys.argv[0][2:dash_loc] + '.v'
filename_son    = sys.argv[0][2:-3]

print >> sys.stderr, 'generating', filename_son, 'used in', filename_father

sys.exit(0)

size = 60;
seed_size = 8;

#cell  cell0 (clk, op, 0, rule, state[4:3], state[1:0], state[2]);

for i in xrange(size - 1, -1, -1):

  r = ((i - 2 + size) % size, (i - 1 + size) % size)
  l = ((i + 1) % size, (i + 2) % size)

  if l[0] + 1 == l[1]:
    left = 'state[%d:%d]' % (l[1], l[0])
  else:
    left = '{state[%d], state[%d]}' % (l[1], l[0])

  if r[0] == r[1] - 1:
    right = 'state[%d:%d]' % (r[1], r[0])
  else:
    right = '{state[%d], state[%d]}' % (r[1], r[0])

  cell = 'state[%d]' % (i)

  if i < seed_size:
    seed = 'seed[%d]' % (i)
  else:
    seed = '0';

  print 'cell cell' + repr(i) + '(clk, op, ' + seed + ', rule, ' + \
        left + ', ' + cell + ', ' + right + ');'
