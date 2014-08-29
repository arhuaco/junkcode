''' Generate random JSON-like objects for testing. '''

import random
import sys

def next_int():
    ''' Get the next int to use as key or value for the JSON. '''
    return random.randint(1, 100003)

def make_random_json_like_hash(max_depth=3, max_keys=3):
    ''' Generate the random JSON-like object. '''
    if max_depth == 0:
        return next_int()
    ret = {}
    for _ in range(random.randint(1, max_keys)):
        next_depth = random.randint(0, max_depth - 1)
        ret[next_int()] = make_random_json_like_hash(next_depth)
    return ret


def json_like_checksum(json_like):
    ''' Get the checksum of the JSON-like object. '''
    if isinstance(json_like, (int)):
        return json_like
    checksum = 0
    for key, value in json_like.items():
        checksum += (key + json_like_checksum(value)) % 2932031007403
    return checksum

def main():
    ''' Out main function. '''
    test_json = make_random_json_like_hash()
    print('json_like_dict: {}'.format(test_json), file=sys.stderr)
    print('checksum: {}'.format(json_like_checksum(test_json)), file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
