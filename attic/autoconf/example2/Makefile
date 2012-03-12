all: example

example: main.c imprimir.o
	gcc main.c -o example -Wall -I. imprimir.o

imprimir.o: imprimir.c imprimir.h
	gcc -c imprimir.c -Wall -I.

.PHONY: clean

clean:
	rm -f example imprimir.o
