#!/usr/bin/env python

# Generate a 'vanity' bitcoin Address.
# This is very slow, but easy to audit. There are fast alternatives like:
# https://en.bitcoin.it/wiki/Vanitygen

from bitcoin import * # https://github.com/vbuterin/pybitcointools
from Crypto import Random
import datetime
import time
import sys

def validate_prefix(prefix):
  for c in prefix:
    if c not in '123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz':
      print >> sys.stderr, 'Invalid char in prefix: {}'.format(c)
      sys.exit(1)

prefix = '1Love' # Desired 'vanity' prefix.

# Harsh validation. Let's not be greedy and accept an approximate solution (mixed case).
validate_prefix(prefix)
prefix = prefix.lower()
validate_prefix(prefix)

out_date = datetime.datetime.fromtimestamp(int(time.time())).strftime('%Y-%m-%d_%H-%M-%S')
out  = open('{}_{}.log'.format(prefix, out_date),  'w')

while True:
  priv = Random.get_random_bytes(256 / 8)
  pub = privtopub(priv)
  addr = pubtoaddr(pub)
  if addr.lower().startswith(prefix):
    print 'priv:', priv.encode('hex')
    print 'addr:', addr
    print
    print >> out, 'priv:', priv.encode('hex')
    print >> out, 'addr:', addr
    print >> out
    out.flush()
