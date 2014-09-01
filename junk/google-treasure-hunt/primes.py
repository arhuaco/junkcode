import sys
import math

def sieveOfErat(end):
    """ Let's use the simpler Sieve of Eratostenes
        Code from: http://krenzel.info/?p=83
        Note that using psyco (http://psyco.sourceforge.net/)
        the Sieve of Atkin is faster as we expect it to be.
        Let's use this simpler version. """
    if end < 2: return []
    lng = ((end / 2) - 1 + end % 2)
    sieve = [True]*(lng+1)
    for i in range(int(math.sqrt(end)) >> 1):
        if not sieve[i]: continue
        for j in range( (i*(i + 3) << 1) + 3, lng, (i << 1) + 3):
            sieve[j] = False
    primes = [2]
    primes.extend([(i << 1) + 3 for i in range(lng) if sieve[i]])
    return primes

def consecutive_sum2(primes, N, start = 2, theset = set()):
  first = 0
  while primes[first] != start:
    first += 1
  last = first + N - 1
  sum = 0
  ret = []
  for i in xrange (first, last + 1):
    sum += primes[i]
  if sum in theset:
    ret.append(sum)
  while last < len(primes) - 1:
    sum -= primes[first]
    first += 1
    last += 1
    sum += primes[last]
    if sum in theset:
      ret.append(sum)
  return ret

primes = sieveOfErat(10000000)
surv = set(primes)

for i in [689, 201, 63, 3]:
  print len(surv)
  surv = set(consecutive_sum2(primes, i, 2, surv))

print 'solution:', min(surv)
