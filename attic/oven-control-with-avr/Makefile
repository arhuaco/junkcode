MCU=atmega16
CC=avr-gcc
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
#-------------------
all: slave.hex
#-------------------
slave.hex : slave.out 
	$(OBJCOPY) -R .eeprom -O ihex slave.out slave.hex 
slave.out : slave.o 
	$(CC) $(CFLAGS) -o slave.out -Wl,-Map,slave.map slave.o  -Wall
slave.o : slave.c 
	$(CC) $(CFLAGS) -Os -c slave.c 
# you need to erase first before loading the program.
# load (program) the software into the eeprom:
load: slave.hex
	uisp -dlpt=/dev/parport0 --erase  -dprog=dapa
	uisp -dlpt=/dev/parport0 --upload if=slave.hex -dprog=dapa  -v=3 --hash=32
#-------------------
.PHONY:clean
clean:
	rm -f *.o *.map *.out slave.hex
#-------------------
