CFLAGS=-I. -Wall -pedantic -O2

all: clock

clock: fonts.o fonts.h clock.c 
	gcc clock.c -o clock fonts.o $(CFLAGS) -lncurses
fonts.o: fonts.c fonts.h
	gcc -c fonts.c $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o clock
