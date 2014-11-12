def fact(x):
    ''' The factorial. Assuming x >= 0. '''
    if x <= 1:
        return 1
    return x * fact(x - 1)

FACT = [fact(i) for i in range(10)]

def get_digits(number):
    while number > 0:
        yield number % 10
        number //= 10

def FindMatches():
    # I din't user an upper bound. Check the thread for ideas once you solve the problem.
    for num in range(3,10000000000):
        fact_sum = 0
        for digit in get_digits(num):
            fact_sum += FACT[digit]
        if fact_sum == num:
            print(num)

FindMatches()
