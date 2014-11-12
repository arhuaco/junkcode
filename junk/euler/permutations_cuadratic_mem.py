''' Prints all permutations. Not in lexicografic order. Uses cuadratic memory! '''
def Permutations(vec):
    if len(vec) <= 1:
        yield vec
    else:
        for rest in Permutations(vec[1:]):
            for split in range(len(rest) + 1):
                yield rest[:split] + [vec[0]] + rest[split:]
for perm in Permutations([1, 2, 3, 4]):
   print(perm)
