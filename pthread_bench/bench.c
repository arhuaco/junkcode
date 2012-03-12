#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>

// Creating and destroying threads to force context switches
const int N_CREATION_DESTROY = 1000 * 100;
// Using conditions to force context switches
const int N_CONDITION = 1000 * 1000;

/* Creation and destruction of threads. This implies a context switch
 * between the main thead and the new one */

void *do_nothing(void *data)
{
    pthread_exit(NULL);
}

void test_create_and_destroy(void)
{
    int i;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (i = 0; i < N_CREATION_DESTROY; i++) {
        pthread_t p_thread;

        if (pthread_create(&p_thread, &attr, do_nothing, (void *) NULL))
            perror("pthread_create");

        if (pthread_join(p_thread, NULL))
            perror("pthread_join");

    }
}

/* forcing context switches with conditions */

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_counter_parity_changed = PTHREAD_COND_INITIALIZER;

int parity_count = 0;

void *do_count_with_parity(void *data)
{
    int zero_or_one = (int) data;

    pthread_mutex_lock(&counter_mutex);

    while (parity_count < N_CONDITION) {
        while (parity_count % 2 == zero_or_one)
            pthread_cond_wait(&cond_counter_parity_changed,
                              &counter_mutex);

        parity_count++;

        /* printf ("me%d: before:%d after:%d\n", zero_or_one, parity_count,
           parity_count + 1); */

        pthread_cond_signal(&cond_counter_parity_changed);
    }

    pthread_mutex_unlock(&counter_mutex);

    pthread_exit(NULL);
}

void test_conditions(void)
{
    int i;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t p_thread[2];

    for (i = 0; i < 2; ++i)
        if (pthread_create
            (&p_thread[i], &attr, do_count_with_parity, (void *) i))
            perror("pthread_create");

    for (i = 0; i < 2; ++i)
        if (pthread_join(p_thread[i], NULL))
            perror("pthread_join");
}


int main(int argc, char *argv[])
{
    typedef struct {
        char *desc;
        void (*function) (void);
    } test_case;

    test_case tests[] = {
        {"Create and destroy", test_create_and_destroy},
        {"Test conditions   ", test_conditions},
        {NULL, NULL}
    };

    int i;

    struct tms current;

    clock_t user_usecs, system_usecs;

    float user_secs, system_secs, wall_clock_secs;
    float total_user_secs = 0.0, total_system_secs = 0.0,
        total_wall_clock_secs = 0.0;

    struct timeval t1, t2, tr;

    float norm;
    norm = (float) sysconf(_SC_CLK_TCK);

    for (i = 0; tests[i].desc; ++i) {
        printf("Test %s : ", tests[i].desc);
        fflush(stdout);

        times(&current);
        user_usecs = current.tms_utime / norm;
        system_usecs = current.tms_stime / norm;

        gettimeofday(&t1, NULL);

        tests[i].function();

        times(&current);
        user_secs = current.tms_utime / norm - user_usecs;
        system_secs = current.tms_stime / norm - system_usecs;

        gettimeofday(&t2, NULL);

        timersub(&t2, &t1, &tr);
        wall_clock_secs = (tr.tv_sec + tr.tv_usec / 1.0e6);

        printf("real %f user %f sys %f\n", wall_clock_secs, user_secs,
               system_secs);

        total_user_secs += user_secs;
        total_system_secs += system_secs;
        total_wall_clock_secs += wall_clock_secs;
    }

    printf("Total : real %f user %f sys %f\n", total_wall_clock_secs,
           total_user_secs, total_system_secs);

    return 0;
}
