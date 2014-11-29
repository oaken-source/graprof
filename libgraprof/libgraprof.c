
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


#include "libgraprof.h"

#include "highrestimer.h"
#include "mallhooks.h"
#include "instrument.h"

#include "common/tracebuffer.h"

#include <grapes/feedback.h>
#include <grapes/file.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

FILE *libgraprof_out = NULL;
char *libgraprof_filename = NULL;

unsigned int libgraprof_hooked = 0;

static void
__attribute__ ((constructor))
libgraprof_init ()
{
  int errsv = errno;

  libgraprof_filename = getenv("GRAPROF_OUT");

  if (libgraprof_filename)
    {
      int errnum = errno;
      unlink(libgraprof_filename);
      errno = errnum;
      libgraprof_out = fopen(libgraprof_filename, "wb");
    }

  if (libgraprof_out)
    libgraprof_install_hooks();

  errno = errsv;
}

static void
__attribute__ ((destructor))
libgraprof_fini ()
{
  int errsv = errno;

  if (libgraprof_out)
  {
    libgraprof_uninstall_hooks();

    tracebuffer_packet p = {
      .type = 'E',
      .exit_all = { { 0 } },
      .time = highrestimer_get()
    };
    md5_digest(p.exit_all.digest, "/proc/self/exe");
    tracebuffer_append(p);
    tracebuffer_finish();

    fclose(libgraprof_out);
  }

  errno = errsv;
}

void
libgraprof_install_hooks (void)
{
  libgraprof_hooked = 1;
}

void
libgraprof_uninstall_hooks (void)
{
  libgraprof_hooked = 0;
}
