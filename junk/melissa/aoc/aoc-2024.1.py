with open("input") as input_f:
    line = input_f.readline().strip()
    col1 = []
    col2 = []
    while line:
        x = line.split(" ")[0]
        y = line.split(" ")[-1]
        col1.append(int(x))
        col2.append(int(y))
        line = input_f.readline().strip()

col1.sort()
col2.sort()

suma = 0

for idx in range(len(col1)):

    suma = suma + max(col2[idx], col1[idx]) - min(col2[idx], col1[idx])

print(suma)
