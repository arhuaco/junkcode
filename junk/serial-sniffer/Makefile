all: serial-test rs232-sniffer

CFLAGS=-Wall -I.

rs232-sniffer: rs232-sniffer.c serial_helper.o
	gcc -Wall -I. rs232-sniffer.c serial_helper.o -o rs232-sniffer

serial-test: serial_helper.c serial_helper.h
	gcc -Wall serial_helper.c -o serial-test -DSERIAL_MAIN

serial_helper.o: serial_helper.c serial_helper.h

.PHONY: clean

clean:
	rm -rf *.o *~ serial-test rs232-sniffer
