
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


#pragma once

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <grapes/util.h>

/* expose the name of the tracefile
 */
extern char *libgraprof_filename;

/* indicate wether profiling hooks are active
 */
extern unsigned int libgraprof_hooked;

/* activate all profiling hooks
 */
#define libgraprof_install_hooks() libgraprof_hooked = 1

/* deactivate all profiling hooks
 */
#define libgraprof_uninstall_hooks() libgraprof_hooked = 0

/* convenience macro for repetitive tracebuffer append stuff
 */
#define __checked_tracebuffer_append(P) \
    do { \
      (P)->time = highrestimer_get(); \
      int res = tracebuffer_append(P); \
      feedback_assert_wrn(!res, "libgraprof: `%s': unable to write trace data", libgraprof_filename); \
    } while (0)


#ifndef LIBGRAPROF_TIMING
#  define LIBGRARPOF_TIMING 0
#endif

#if LIBGRAPROF_TIMING

  /* the following definitions enable timing evaluation for the
   * instrumentation and allocation hooks. to enable, configure with
   * --enable-libgraprof-timing
   */

  #include <stdio.h>

  #define __hook_prolog \
      unsigned long long s = highrestimer_get()

  extern size_t tracebuffer_index;

  #define __hook_epilog(T) \
      unsigned long long t = highrestimer_get(); \
      max_delay = max(max_delay, t - s - timer_delay); \
      total_delay += t - s - timer_delay; \
      ++count; \
      avg_delay = total_delay / count; \
      printf(" " T " %6llu ns [%6llu ns] [%6llu ns]", t - s - timer_delay, max_delay, avg_delay); \
      if (tracebuffer_index == 0) printf(" [flushed]"); \
      printf("\n") \

  extern unsigned long long timer_delay;
  extern unsigned long long max_delay;
  extern unsigned long long total_delay;
  extern unsigned long long avg_delay;
  extern unsigned long long count;

#else // not LIBGRAPROF_TIMING

  #define __hook_prolog
  #define __hook_epilog(X)

#endif
