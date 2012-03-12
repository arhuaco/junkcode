obj-m += fullex.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f test

test: test.c fullex.h
	gcc -Wall test.c -o test
