' Ejercicio para series de taylor '

import math

def fact(num):
    ' factorial recursivo - para valores pequeños de num '
    if num > 0:
        return num * fact(num - 1)
    assert num == 0 # función no definida para números negativos
    return 1

def taylor(num, cuantas):
    ' computando iteraciones de la serie '
    ret = num # Abajo se usa num, por eso la copia a otra variable.
    suma = False
    for i in range(cuantas):
        exp = 3 + i * 2
        if suma:
            ret += num ** exp / fact(exp)
        else:
            ret -= num ** exp / fact(exp)
        suma = not suma
    return ret

def compara(valor):
    ' compara aproximaciones sucesivas '
    for cuantos in range(0, 10):
        print('digitos=', cuantos)
        seno_libreria_python = math.sin(valor)
        resultado_aproximado_taylor = taylor(valor, cuantos)
        print('seno de', valor * 180 / math.pi, '=', seno_libreria_python)
        print('TAYL de', valor * 180 / math.pi, '=', resultado_aproximado_taylor)
        print('diferecia', abs(seno_libreria_python - resultado_aproximado_taylor), '\n')

compara(0)
compara(math.pi / 4)
compara(math.pi / 2)
