#!/usr/bin/env python

import sys
import VeriGen

def make_cells(args):

  if len(args) < 2:
    print sys.stderr('expected 2 arguments')
    sys.exit(1)

  size = args[0]
  seed_size = args[1]

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
      seed = '1\'b0';

    print 'cell_state cell' + repr(i) + '(clk, op, ' + seed + ', rule, ' + \
          left + ', ' + cell + ', ' + right + ');'

g = VeriGen.VeriGen([make_cells], False)
