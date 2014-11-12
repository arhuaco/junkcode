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

def get_digits(number):
    ret = []
    while number > 0:
        ret.append(number % 10)
        number //= 10
    ret.reverse()
    return ret

def rotations(number):
    digits = get_digits(number)
    ret = []
    for base in range(len(digits)): 
        i = base
        first_time = True
        rot = ''
        while i != base or first_time:
            first_time = False
            rot += str(digits[i]) 
            i = (i + 1) % len(digits)
        ret.append(int(rot))
    return ret

def solve(N):
    ret = []
    primes = simple_sieve(N * 10 + 1)
    primes_set = set(primes)
    for prime in primes:
        if prime >= N:
            break
        all_rot = rotations(prime)
        if sum([1 if rot in primes_set else 0 for rot in all_rot]) == len(all_rot):
            ret.append(prime)
    return ret

solution = solve(1000 * 1000)
print(solution)
print('A:{}'.format(len(solution)))
