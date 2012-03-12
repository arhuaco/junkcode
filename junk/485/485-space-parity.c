#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <asm-i486/ioctls.h>    /* A better file to include? */

#include <termios.h>
#include <strings.h>

int write_space_parity(int fd, char *packet, int len)
{
    static const char parity[256] = {
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
        0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0
    };

    struct termios tio;
    int sum = 0;

    bzero(&tio, sizeof(tio));
    tcgetattr(fd, &tio);

    tio.c_cflag |= PARENB;

    while (len--) {
        /* TODO: Can we only update when needed? */
        if (parity[(int) (*packet)])
            tio.c_cflag |= PARODD;
        else
            tio.c_cflag &= ~PARODD;

        tcsetattr(fd, TCSADRAIN, &tio);
        sum += write(fd, packet++, 1);
    }

    return sum;                 /* better than nothing */
}


struct serial_control_signal {
    int flag;
    char *str;
};

struct serial_control_signal signals[] = {
    {TIOCM_LE, "DSR (data set ready/line enable)"},
    {TIOCM_DTR, "DTR (data terminal ready)"},
    {TIOCM_RTS, "RTS (request to send)"},
    {TIOCM_ST, "Secondary TXD (transmit)"},
    {TIOCM_SR, "Secondary RXD (receive)"},
    {TIOCM_CTS, "CTS (clear to send)"},
    {TIOCM_CAR, "DCD (data carrier detect)"},
    {TIOCM_RNG, "RNG (ring)"},
    {TIOCM_DSR, "DSR (data set ready)"}
};


void signals_show(int fd)
{
    int ret;
    int status;
    int i;

    ret = ioctl(fd, TIOCMGET, &status);

    if (ret != 0) {
        perror("ioctl");
        exit(1);
    }

    for (i = 0; i < sizeof(signals) / sizeof(struct serial_control_signal);
         ++i)
        printf("%s %s\n", (status & signals[i].flag) ? " + " : " - ",
               signals[i].str);
}

void disable_rx_enable_tx(int fd)
{
    int status;
    ioctl(fd, TIOCMGET, &status);
    status |= TIOCM_RTS;
    if (ioctl(fd, TIOCMSET, &status)) {
        perror("ioctl");
        exit(1);
    }
}

void disable_tx_enable_rx(int fd)
{
    int status;
    ioctl(fd, TIOCMGET, &status);
    status &= ~TIOCM_RTS;
    ioctl(fd, TIOCMSET, &status);
}

int serial_bus_init(void)
{
    int fd;
    struct termios attr;

    if ((fd = open("/dev/ttyS1", O_RDWR | O_EXCL)) <= 0) {
        fprintf(stderr, "Unable to open 485 port\n");
        exit(1);
    }

    if (tcgetattr(fd, &attr) < 0) {
        fprintf(stderr, "Call to tcgetattr failed: %s\n", strerror(errno));
        exit(1);
    }

    attr.c_lflag = 0;
    attr.c_oflag = 0;
    attr.c_cc[VTIME] = 0;       /* timeout in tenths of a second */
    attr.c_cc[VMIN] = 1;        /* Wait for a single char */
    attr.c_iflag = 0;

/*
 * The non-standard flag CMSPAR is supported by Linux.
 * It is used for Stick Parity.
 */

#define CMSPAR   010000000000
    attr.c_cflag = CLOCAL | CREAD | CS8 | CMSPAR | PARENB;
    attr.c_cflag &= ~PARODD;    /*TODO: not needed */

    cfsetispeed(&attr, B57600);
    cfsetospeed(&attr, B57600);

    if (tcsetattr(fd, TCSAFLUSH, &attr) < 0) {
        fprintf(stderr, "Call to tcsetattr failed: %s\n", strerror(errno));
        exit(1);
    }

    disable_tx_enable_rx(fd);   /* using automatic flow control */
    return fd;
}

int main(int argc, char **argv)
{
    int r;

    char cmd[] = { 8, 0 };
    char e;
    int ret;

    r = serial_bus_init();

    if (r < 0) {
        fprintf(stderr, "Couldn't open serial\n");
        exit(1);
    }

    signals_show(r);

#define AUTO_FLOW_CONTROL
#ifdef AUTO_FLOW_CONTROL
/* With this option the RTS line will be automatically
 * set when needed */
    disable_tx_enable_rx(r);
    ret = write_space_parity(r, cmd, 2);
#else
    disable_rx_enable_tx(r);
    ret = write_space_parity(r, cmd, 2);
    disable_tx_enable_rx(r);
#endif

    if (!ret) {
        perror("write");
        exit(1);
    }

    printf("AFTER ****\n");
    signals_show(r);
    printf("DATA? ****\n\n");

    while (1) {
        if (!read(r, &e, 1)) {
            perror("read");
            exit(1);
        }
        printf("reply %xx(%d)(print:%c)\n", e, e, isprint(e) ? e : '.');
    }

    close(r);                   /* not reached */

    return 0;
}
