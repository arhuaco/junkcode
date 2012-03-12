
#TODO: make a better makefile

TARGETS=alternate neural

all: $(TARGETS)

LDFLAGS=-lmiga
CFLAGS=-Wall -I. -O3

%: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ 

neural: neural-net.o neural-unit.o neural.o
	gcc $(CFLAGS) $(LDFLAGS)  neural.c neural-net.o neural-unit.o -o $@


.PHONY:clean

clean:
	rm -f $(TARGETS) *.o 
