
#include "highrestimer.h"

#include <time.h>

static struct timespec highrestimer_ts;

static unsigned long long __highrestimer_get_first();
static unsigned long long __highrestimer_get();

static unsigned long long (*_highrestimer_get)() = &__highrestimer_get_first;

unsigned long long
highrestimer_get ()
{
  return (*_highrestimer_get)();
}

static unsigned long long
__highrestimer_get_first ()
{
  clock_gettime(CLOCK_MONOTONIC_RAW, &highrestimer_ts);
  _highrestimer_get = &__highrestimer_get;
  
  return 0;
}

static unsigned long long
__highrestimer_get ()
{
  struct timespec tmp_ts;

  clock_gettime(CLOCK_MONOTONIC_RAW, &tmp_ts);

  unsigned long long res = tmp_ts.tv_sec - highrestimer_ts.tv_sec;
  res *= 1000000000;
  res += tmp_ts.tv_nsec - highrestimer_ts.tv_nsec;

  return res;
}
