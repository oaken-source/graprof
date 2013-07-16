
#include "highrestimer.h"

#include <time.h>

struct timespec highrestimer_ts;

void
__attribute__ ((constructor))
highrestimer_init ()
{
  clock_gettime(CLOCK_MONOTONIC_RAW, &highrestimer_ts);
}

unsigned long
highrestimer_get ()
{
  struct timespec tmp_ts;

  clock_gettime(CLOCK_MONOTONIC_RAW, &tmp_ts);

  return (tmp_ts.tv_nsec - highrestimer_ts.tv_nsec) / 1000 + (tmp_ts.tv_sec - highrestimer_ts.tv_sec) * 1000000;
}
