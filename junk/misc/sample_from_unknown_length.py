'''
Sample from sequence of unknown length.
Possible output:
digit 0: 0.301
digit 1: 0.297
digit 2: 0.298
digit 3: 0.302
digit 4: 0.3
digit 5: 0.3
digit 6: 0.299
digit 7: 0.3
digit 8: 0.3
digit 9: 0.3
'''

import random
import sys

if sys.version_info[0] < 3:
    raise Exception('Use Python 3')

def test(next_item, sample_size):
    ' The actual sampler '
    sample = []
    count = 0
    for item in next_item:
        count += 1
        if len(sample) < sample_size:
            sample.append(item)
        elif random.random() < sample_size / count:
            sample[random.randint(0, len(sample) - 1)] = item
    return sample

def main():
    ' main. duh. '
    random.seed()
    counts = {}
    for total in range(100000):
        for num in test(range(10), 3):
            if num in counts:
                counts[num] += 1
            else:
                counts[num] = 0
    for digit in sorted(counts.keys()):
        print(f'digit {digit}: {round(counts[digit] / total, 3)}')

if __name__ == "__main__":
    main()
