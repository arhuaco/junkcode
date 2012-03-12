#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>
#include <getopt.h>
#include <termios.h>

/*  This code started as a streamlined version of
 *  a C++ file that is part of a project which is
 *  is Copyright (c) 2005 Dave Hylands.
 *  http://www.davehylands.com/Software/BootHost/
 */

#define SERIAL_BAD_BAUDRATE -1000
#define SERIAL_BAD_PORT -1001
#define SERIAL_GEN_ERROR -1002

static struct {
    speed_t speed;
    unsigned baudRate;
} BaudTable[] = {
    {
    B50, 50}, {
    B75, 75}, {
    B110, 110}, {
    B134, 134}, {
    B150, 150}, {
    B200, 200}, {
    B300, 300}, {
    B600, 600}, {
    B1200, 1200}, {
    B1800, 1800}, {
    B2400, 2400}, {
    B4800, 4800}, {
    B9600, 9600}, {
    B19200, 19200}, {
    B38400, 38400}, {
    B57600, 57600}, {
    B115200, 115200}, {
    B230400, 230400}
};

#define ARRAY_LEN(x)    ( sizeof( x ) / sizeof( x[ 0 ]))

int serial_open(char *portStr, int try_baud_rate, int verbose)
{
    speed_t baudRate = B0;
    int gPortFd = -1;
    int i;

    struct termios attr;

    for (i = 0; i < ARRAY_LEN(BaudTable); i++) {
       if (BaudTable[i].baudRate == try_baud_rate) {
            baudRate = BaudTable[i].speed;
            break;
        }
    }

    if (baudRate == B0) {
        if (verbose)
          fprintf(stderr, "Unrecognized baud rate: '%d'\n", try_baud_rate);
        return SERIAL_BAD_BAUDRATE;
    }

    if ((gPortFd = open(portStr, O_RDWR | O_EXCL)) < 0) {
        if (verbose)
          fprintf(stderr, "Unable to open serial port '%s': %s\n", portStr,
                strerror(errno));
        return SERIAL_BAD_PORT;
    }

    if (tcgetattr(gPortFd, &attr) < 0) {
        if (verbose)
          fprintf(stderr, "Call to tcgetattr failed: %s\n", strerror(errno));
        return SERIAL_GEN_ERROR;
    }

    attr.c_iflag = 0;
    attr.c_oflag = 0;
    attr.c_cflag = CLOCAL | CREAD | CS8;
    attr.c_lflag = 0;
    attr.c_cc[VTIME] = 0;       /* timeout in tenths of a second */
    attr.c_cc[VMIN] = 1;        /* Only wait for a single char */

    cfsetispeed(&attr, baudRate);
    cfsetospeed(&attr, baudRate);

    if (tcsetattr(gPortFd, TCSAFLUSH, &attr) < 0) {
        if (verbose)
          fprintf(stderr, "Call to tcsetattr failed: %s\n", strerror(errno));
        return SERIAL_GEN_ERROR;
    }

    return gPortFd;
}

#ifdef SERIAL_MAIN
int
main(int argc, char **argv)
{
 int r;
 r = serial_open("/dev/ttyUSB0", 9600, 1);
 printf ("R = %d\n", r);
 return 0;
}
#endif
