#!/usr/bin/env python

from __future__ import absolute_import, division, print_function, unicode_literals

import random
import sys

def next_int():
  return random.randint(1, 100003)

def make_random_json_like_hash(max_depth=3, max_keys=3):
    if max_depth == 0:
        return next_int()
    ret = {}
    for idx in xrange(random.randint(1, max_keys)):
        next_depth = random.randint(0, max_depth - 1)
        ret[next_int()] = make_random_json_like_hash(next_depth)
    return ret


def json_like_checksum(json_like):
    if isinstance(json_like, (int, long)):
        return json_like
    checksum = 0
    for key, value in json_like.items():
      checksum += (key + json_like_checksum(value)) % 2932031007403
    return checksum

if __name__ == "__main__":
    test_json = make_random_json_like_hash()
    print('json_like_dict: {}'.format(test_json), file=sys.stderr)
    print('checksum: {}'.format(json_like_checksum(test_json)), file=sys.stderr)
    sys.exit(0)
