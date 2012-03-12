#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

typedef unsigned long long timestamp_t;

static timestamp_t
get_timestamp ()
{
  struct timeval now;
  gettimeofday (&now, NULL);
  return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

int
main(int argc, char *argv)
{
  timestamp_t t0 = get_timestamp();
  usleep(500000);
  timestamp_t t1 = get_timestamp();

  printf("%Lg seconds", (long double)(t1 - t0) / 1000000L);

  return 0;
}
