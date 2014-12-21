
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


#define _GNU_SOURCE

#include "libgraprof.h"

#include "highrestimer.h"

#include "common/tracebuffer.h"

#include <grapes/feedback.h>

#include <limits.h>
#include <stdlib.h>
#include <link.h>

unsigned long long timer_delay = 29;
unsigned long long max_delay = 0;
unsigned long long total_delay = 0;
unsigned long long avg_delay;
unsigned long long count = 0;

char *libgraprof_filename = NULL;

unsigned int libgraprof_hooked = 0;

static int __may_fail
libgraprof_register_object_file (struct dl_phdr_info *info, __unused size_t size, __unused void *data)
{
  if (!info->dlpi_addr)
    return 0;

  char *filename = realpath(info->dlpi_name, NULL);
  if (!filename)
    return 0;

  __checked_call(0 == tracebuffer_append_header(filename, info->dlpi_addr));

  return 0;
}

static int __may_fail
libgraprof_write_trace_header (void)
{
  char *filename;
  __checked_call(NULL != (filename = realpath("/proc/self/exe", NULL)));

  __checked_call(0 == tracebuffer_append_header(filename, 0));
  __checked_call(0 == dl_iterate_phdr(libgraprof_register_object_file, NULL));

  __checked_call(0 == tracebuffer_finish_header());

  return 0;
}

static void
__attribute__ ((constructor))
libgraprof_init ()
{
  int errnum = errno;

  do
    {
      libgraprof_filename = getenv("GRAPROF_OUT");
      if (!libgraprof_filename)
        break;

      int res = tracebuffer_openw(libgraprof_filename);
      if (res)
        {
          feedback_warning("libgraprof: `%s': unable to open", libgraprof_filename);
          break;
        }

      res = libgraprof_write_trace_header();
      if (res)
        {
          feedback_warning("libgraprof: `%s:' unable to write trace header", libgraprof_filename);
          break;
        }

      libgraprof_install_hooks();
    }
  while (0);

  errno = errnum;
}

static void
__attribute__ ((destructor))
libgraprof_fini ()
{
  int errnum = errno;

  if (libgraprof_hooked)
  {
    libgraprof_uninstall_hooks();
    int res = tracebuffer_close();
    feedback_assert_wrn(!res, "libgraprof: `%s': unable to close", libgraprof_filename);
  }

  errno = errnum;
}
