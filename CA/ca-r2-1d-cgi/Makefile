CFLAGS=-Wall -pedantic -O2 -I.
LDFLAGS=-lgd

all: ca-r2-1d-to-img

ca-r2-1d-to-img: ca-r2-1d-to-img.c util.h caiterate.o
	gcc $(CFLAGS) $(LDFLAGS) ca-r2-1d-to-img.c caiterate.o -o ca-r2-1d-to-img -lgd


caiterate.o: caiterate.c caiterate.h util.h
	gcc $(CFLAGS) -c caiterate.c 

.PHONY: clean

clean:
	rm -f caiterate.o ca-r2-1d-to-img
