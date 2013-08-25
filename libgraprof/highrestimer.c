
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

#include <grapes/util.h>
#include <grapes/feedback.h>

static struct timespec highrestimer_ts;

#define HIGHRESTIMER_CLOCK CLOCK_MONOTONIC_RAW

static void
__attribute__((constructor))
highrestimer_init ()
{
  int res = clock_gettime(HIGHRESTIMER_CLOCK, &highrestimer_ts);
  if (res)
    {
      errno = 0;
      feedback_warning("selected clock_id is not valid. Time measurement will not work.");
    }
}

unsigned long long
highrestimer_get ()
{
  struct timespec tmp_ts;

  int res = clock_gettime(HIGHRESTIMER_CLOCK, &tmp_ts);
  assert_inner(!res, "clock_gettime");

  unsigned long long t = tmp_ts.tv_sec - highrestimer_ts.tv_sec;
  t *= 1000000000;
  t += tmp_ts.tv_nsec - highrestimer_ts.tv_nsec;

  return t;
}

