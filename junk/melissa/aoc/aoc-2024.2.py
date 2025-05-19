with open("input") as input_f:
    line = input_f.readline().strip()
    col1 = []
    count2 = {}
    while line:
        x = line.split(" ")[0]
        y = int(line.split(" ")[-1])
        col1.append(int(x))
        if y in count2:
            count2[y] += 1
        else:
            count2[y] = 1
        line = input_f.readline().strip()

suma = 0

for num_izq in col1:
    if num_izq in count2:
        multip = count2[num_izq]
        inc = multip * num_izq
        suma += inc

print(suma)
