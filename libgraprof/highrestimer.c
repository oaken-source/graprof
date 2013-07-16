
#include "highrestimer.h"

#include <time.h>

struct timespec highrestimer_ts;

void
__attribute__ ((constructor))
highrestimer_init ()
{
  clock_gettime(CLOCK_MONOTONIC_RAW, &highrestimer_ts);
}

unsigned long long
highrestimer_get ()
{
  struct timespec tmp_ts;

  clock_gettime(CLOCK_MONOTONIC_RAW, &tmp_ts);

  unsigned long long res = tmp_ts.tv_sec - highrestimer_ts.tv_sec;
  res *= 1000000000;
  res += tmp_ts.tv_nsec - highrestimer_ts.tv_nsec;

  return res;
}
