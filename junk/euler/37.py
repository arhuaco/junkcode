import math

def simple_sieve(N):
    table = [True] * N
    table[0] = False
    table[1] = False
    limit = int(math.sqrt(N)) + 1
    current = 2
    while current <= limit:
        if table[current]:
            factor = 2
            while factor * current < N:
                table[factor * current] = False
                factor += 1
        current += 1
    ret = []
    for i in range(N):
        if table[i]:
            ret.append(i)
    return ret

def truncations(number):
   ret = []
   digits = str(number)
   for i in range(1, len(digits)):
       ret.append(int(digits[i:]))
       ret.append(int(digits[:i]))
   return ret

def solve(N):
    ret = []
    primes = simple_sieve(N * 10 + 1)
    primes_set = set(primes)
    for prime in primes:
        if prime >= N:
            break
        if prime <= 7:
            continue
        all_trunc = truncations(prime)
        if sum([1 if rot in primes_set else 0 for rot in all_trunc]) == len(all_trunc):
            ret.append(prime)
    return ret

solution = solve(1000 * 1000)
print(solution)
print('A:{}'.format(sum(solution)))
