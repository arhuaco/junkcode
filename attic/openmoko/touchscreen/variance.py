#!/usr/bin/python

import math

while True:
    v = map(float, raw_input().split("  "))

    sum = 0
    for s in v:
        sum += s
    avg = sum / float(len(v))
    std = 0
    for s in v:
        std += (avg - s) ** 2
    std /= float(len(v) - 1)

    print "avg:", avg
    print "std:", math.sqrt(std)
    print "std^2:", std
