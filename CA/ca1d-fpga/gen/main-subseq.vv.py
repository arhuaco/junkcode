#!/usr/bin/env python

import sys
import VeriGen

def make_cells(args):

  if len(args) != 1:
    print sys.stderr('expected 1 argument')
    sys.exit(1)

  l = args[0]

  for i in xrange(0, l):
    print 'subsequence_counter count%d (clk, op, state[%d]);' % (i, i)

g = VeriGen.VeriGen([make_cells], False)
