CFLAGS:=-std=gnu99 -O3 -fomit-frame-pointer -fstrict-aliasing -ffast-math  -fno-schedule-insns -fno-loop-optimize -Wall
LDFLAGS:=-lm

all: bench cfft

bench: distance.o

clean:
	rm -f bench cfft *.o
