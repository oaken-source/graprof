
#include "highrestimer.h"

#include <time.h>

static struct timespec highrestimer_ts;

static unsigned long long highrestimer_get_first();
static unsigned long long highrestimer_get_impl();

static unsigned long long (*highrestimer_get_ptr)() = &highrestimer_get_first;

#define HIGHRESTIMER_CLOCK CLOCK_MONOTONIC_RAW

static unsigned long long
highrestimer_get_first ()
{
  clock_gettime(HIGHRESTIMER_CLOCK, &highrestimer_ts);
  highrestimer_get_ptr = &highrestimer_get_impl;
  
  return 0;
}

static unsigned long long
highrestimer_get_impl ()
{
  struct timespec tmp_ts;

  clock_gettime(HIGHRESTIMER_CLOCK, &tmp_ts);

  unsigned long long res = tmp_ts.tv_sec - highrestimer_ts.tv_sec;
  res *= 1000000000;
  res += tmp_ts.tv_nsec - highrestimer_ts.tv_nsec;

  return res;
}

unsigned long long
highrestimer_get ()
{
  return (*highrestimer_get_ptr)();
}

