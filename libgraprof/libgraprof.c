
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


#include "libgraprof.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <grapes/feedback.h>
#include <grapes/md5.h>

#include "highrestimer.h"
#include "mallhooks.h"
#include "instrument.h"
#include "buffer.h"

FILE *libgraprof_out = NULL;
void *libgraprof_buf = NULL;
unsigned long libgraprof_bufsize = 0;
char *libgraprof_filename = NULL;

static void
__attribute__ ((constructor))
libgraprof_init ()
{
  int errsv = errno;

  libgraprof_filename = getenv("GRAPROF_OUT");

  if (libgraprof_filename)
    {
      libgraprof_out = fopen(libgraprof_filename, "w");
    }

  if (libgraprof_out)
    {
      mallhooks_install_hooks();
      instrument_install_hooks();
    }

  errno = errsv;
}

static void
__attribute__ ((destructor))
libgraprof_fini ()
{
  int errsv = errno;

  if (libgraprof_out)
  {
    mallhooks_uninstall_hooks();

    buffer_enlarge(sizeof(char) + sizeof(unsigned long long));
    buffer_append(char, 'E');
    buffer_append(unsigned long long, highrestimer_get());

    unsigned char md5[MD5_DIGEST_LENGTH] = { 0 };
    int res = md5_digest_file("/proc/self/exe", md5);
    feedback_assert_wrn(!res, "libgraprof: /proc/self/exe");

    size_t n;
    n = fwrite(md5, 1, MD5_DIGEST_LENGTH, libgraprof_out);
    feedback_assert_wrn(n == MD5_DIGEST_LENGTH, "libgraprof: %s", libgraprof_filename);

    n = fwrite(&libgraprof_bufsize, sizeof(unsigned long), 1, libgraprof_out);
    feedback_assert_wrn(n == 1, "libgraprof: %s", libgraprof_filename);

    n = fwrite(libgraprof_buf, 1, libgraprof_bufsize, libgraprof_out);
    feedback_assert_wrn(n == libgraprof_bufsize, "libgraprof: %s", libgraprof_filename);

    fclose(libgraprof_out);
  }

  buffer_destroy();
  errno = errsv;
}
