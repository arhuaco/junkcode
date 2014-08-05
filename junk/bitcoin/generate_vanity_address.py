#!/usr/bin/env python

# Generate a 'vanity' bitcoin Address.
# This is very slow, but easy to audit. There are fast alternatives like:
# https://en.bitcoin.it/wiki/Vanitygen

from bitcoin import * # https://github.com/vbuterin/pybitcointools
from Crypto import Random # python-crypto package, or https://www.dlitz.net/software/pycrypto/
import datetime
import time
import sys

def validate_prefix(prefix):
  for c in prefix:
    if c not in '123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz':
      print >> sys.stderr, 'Invalid char in prefix: {}'.format(c)
      sys.exit(1)

prefix = '1test' # Desired 'vanity' prefix.

# TODO: Improve validation and matching. Prefixes such as '1Love' should be doable.
validate_prefix(prefix)
prefix = prefix.lower()
validate_prefix(prefix)

out_date = datetime.datetime.fromtimestamp(int(time.time())).strftime('%Y-%m-%d_%H-%M-%S')
out  = open('{}_{}.log'.format(prefix, out_date),  'w')

print 'Searching for prefix:', prefix
print 'Good luck. This is slow.'

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
