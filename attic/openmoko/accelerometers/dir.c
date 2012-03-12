#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

struct input_event {
    struct timeval time;
    uint16_t type;
    uint16_t code;
    int32_t value;
};

int read_all(int fd, char *buf, int count)
{
    int n_read = 0;
    while (n_read != count) {
        int result = read(fd, buf + n_read, count - n_read);
        if (result < 0)
            return result;
        else if (result == 0)
            return n_read;
        n_read += result;
    }
    return n_read;
}


/* http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C */
unsigned long isqrt(x)
unsigned long x;
{
    register unsigned long op, res, one;

    op = x;
    res = 0;

    /* "one" starts at the highest power of four <= than the argument. */
    one = 1 << 30;              /* second-to-top bit set */
    while (one > op)
        one >>= 2;

    while (one != 0) {
        if (op >= res + one) {
            op = op - (res + one);
            res = res + (one << 1);     // <-- faster than 2 * one
        }
        res >>= 1;
        one >>= 2;
    }

    return res;
}



void update(int x, int y)
{
    int n;
    n = isqrt(x * x + y * y);
    if (n) {
        x = (100 * x) / n;
        y = (100 * y) / n;
    } else {
        x = y = 0;
    }
    fprintf(stderr, "[x:%d y:%d]\n", x, y);
}


int main(int argc, char *argv[])
{
    int fd;
    struct input_event ev;
    char *file;
    int x = 0, y = 0;

    assert(16 == sizeof(struct input_event));

    if (argc != 2)
        file = "/dev/input/event3";
    else
        file = argv[1];

    if ((fd = open(file, O_RDONLY)) == -1) {
        perror("open");
        return 1;
    }

    while (1) {
        int ret = read_all(fd, (char *) &ev, sizeof(struct input_event));
        if (ret != sizeof(struct input_event)) {
            fprintf(stderr, "ret == %d\n", ret);
            perror("read");
            return 1;
        }
        /* http://wiki.openmoko.org/wiki/Accelerometer_data_retrieval */
        switch (ev.type) {
        case 0:                /* SYN_REPORT */
            update(x, y);
            break;
        case 3:                /* EV_ABS */
            if (ev.code == 0)
                x = ev.value;
            if (ev.code == 1)
                y = ev.value;
            break;
        case 2:                /* EV_REL */
            fprintf(stderr, "You need a newer kernel\n");
            exit(1);
        default:
            fprintf(stderr, "Unknown event type %d\n", ev.type);
            exit(1);
            printf("type:%u code:%u value:%d\n", ev.type, ev.code,
                   ev.value);
        }
    }

    return 0;
}
