#include <stdio.h>
#include <sys/time.h>
#include <time.h>

int
set_diff_time(long diff)
{
  struct timeval tv;

  if (gettimeofday(&tv, NULL))
    return 1;

  tv.tv_sec += diff;
  if (settimeofday(&tv, NULL))
    return 1;

  return 0;
}

int
main (void)
{
  /* goes back in time 2 seconds */
  if (set_diff_time(-120))
    fprintf(stderr, "Couldn't change the time\n");
  return 0;
}
