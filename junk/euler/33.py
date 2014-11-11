def DigitCancelling():
    for num in range(10, 100):
        for den in range(10, 100):
            if num % 10 == 0 and den % 10 == 0:
                continue # Trivial example.
            if num / den >= 1.0:
                continue # Restriction 
            num_first = num // 10
            num_second = num % 10
            den_first = den // 10
            den_second = den % 10
            if den_first != 0 and num_first == den_second and num_second / den_first == num / den:
                print('{} / {} ?= {} / {}'.format(num, den, num_second, den_first))
            if den_second != 0 and num_second == den_first and num_first / den_second == num / den:
                print('{} / {} ?= {} / {}'.format(num, den, num_first, den_second))
                
DigitCancelling()

'''

Prints:

16 / 64 ?= 1 / 4
19 / 95 ?= 1 / 5
26 / 65 ?= 2 / 5
49 / 98 ?= 4 / 8

Answer: 8 / 800 == 1 / 100.

'''
