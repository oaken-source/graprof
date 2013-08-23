
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013  Andreas Grapentin                                   *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ******************************************************************************/


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
