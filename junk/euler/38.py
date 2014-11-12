# Solve problem 38. Not using an upper bound.

import sys

def is_pantidigital(number):
    bitmap = [0] * 10
    while number > 0:
        digit = number % 10
        if bitmap[digit] > 0 or digit == 0:
            return False
        bitmap[digit] = 1
        number //= 10
    return sum(bitmap[1:]) == 9

while True:
    candidates = set()
    i = 0
    while True:
        i += 1
        j = 1
        to_test = ''
        while len (to_test) < 9:
            to_test += str(i * j)
            if len(to_test) == 9 and is_pantidigital(int(to_test)) and j > 1:
                candidates.add(int(to_test))
                print(i, int(to_test), max(candidates))
            j += 1
