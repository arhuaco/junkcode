#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sched.h>

#define NTESTS 10000

int main(int argc, char *argv[])
{
    struct timeval t1, t2;
    long double elapsed, max = 0.0, min = 1000.0, avg = 0.0;
    struct sched_param param;
    int i;

    if (argc > 1) {
        param.sched_priority = atoi(argv[1]);
        fprintf(stderr, "Using realtime priority: %d\n",
                param.sched_priority);

        if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
            perror("sched_setscheduler failed");
            return -1;
        }
    }

    /* we measure the accuracy of a usleep of 2 miliseconds */
    for (i = 0; i < NTESTS; ++i) {
        gettimeofday(&t1, NULL);
        usleep(2000);
        gettimeofday(&t2, NULL);


        elapsed = (t2.tv_sec - t1.tv_sec) * 1000 +
                  (long double) (t2.tv_usec - t1.tv_usec) / 1000.0L;
        t1 = t2;

        avg += elapsed;
        if (elapsed > max)
            max = elapsed;
        if (elapsed < min)
            min = elapsed;
    }

    avg /= (long double) NTESTS;
    printf("max:%Lg min:%Lg avg:%Lg\n", max, min, avg);

    return 0;
}
