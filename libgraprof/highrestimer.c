
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013 - 2014  Andreas Grapentin                            *
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

#if HAVE_CLOCK_GETTIME

  #include <time.h>

  static struct timespec start;

  #define CLOCK CLOCK_MONOTONIC

#elif HAVE_MACH_ABSOLUTE_TIME

  #include <stdint.h>
  #include <mach/mach.h>
  #include <mach/mach_time.h>
  #include <CoreServices/CoreServices.h>

  static uint64_t start;
  static mach_timebase_info_data_t timebase;

#else

  #error No md5 implementation available.

#endif

static void
__attribute__((constructor))
highrestimer_init ()
{
  #if HAVE_CLOCK_GETTIME

    clock_gettime(CLOCK, &start);

  #elif HAVE_MACH_ABSOLUTE_TIME

    start = mach_absolute_time();
    mach_timebase_info(&timebase);

  #endif
}

unsigned long long
highrestimer_get (void)
{
  #if HAVE_CLOCK_GETTIME

    struct timespec end;
    clock_gettime(CLOCK, &end);

    return ((end.tv_sec - start.tv_sec) * 1000000000) + (end.tv_nsec - start.tv_nsec);

  #elif HAVE_MACH_ABSOLUTE_TIME

    uint64_t elapsed = mach_absolute_time() - start;

    return  elapsed * timebase.numer / timebase.denom;

  #endif
}

