#ifndef __SERIAL_HELPER_H__
#define __SERIAL_HELPER_H__

#define SERIAL_BAD_BAUDRATE -1000
#define SERIAL_BAD_PORT -1001
#define SERIAL_GEN_ERROR -1002

int serial_open(char *port, int try_baud_rate, int verbose);

#endif
