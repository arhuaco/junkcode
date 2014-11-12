def is_pantidigital(number):
    bitmap = [0] * 10
    while number > 0:
        digit = number % 10
        if bitmap[digit] > 0 or digit == 0:
            return False
        bitmap[digit] = 1
        number //= 10
    return sum(bitmap[1:]) == 9

answer = -1
for i in range(1, 100000):
    j = 1
    to_test = ''
    while len (to_test) < 9:
        to_test += str(i * j)
        if len(to_test) == 9 and is_pantidigital(int(to_test)) and j > 1:
            answer = max(answer, int(to_test))
        j += 1

print('A:{}'.format(answer))
