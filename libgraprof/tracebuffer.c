
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


#include "tracebuffer.h"

#include "highrestimer.h"

#include <grapes/feedback.h>

#include <stdio.h>
#include <errno.h>

#define BUFSIZE 1024

static tracebuffer_packet tracebuffer_buffer[BUFSIZE];
static size_t tracebuffer_index = 0;

extern FILE *libgraprof_out;
extern const char *libgraprof_filename;

static void
tracebuffer_flush ()
{
  int errnum = errno;

  size_t res = fwrite(tracebuffer_buffer, sizeof(*tracebuffer_buffer), tracebuffer_index, libgraprof_out);
  feedback_assert_wrn(res == tracebuffer_index, "libgraprof: error writing '%s'", libgraprof_filename);

  tracebuffer_index = 0;

  errno = errnum;
}

void
tracebuffer_append (const tracebuffer_packet *p)
{
  tracebuffer_buffer[tracebuffer_index] = *p;
  tracebuffer_buffer[tracebuffer_index++].time = highrestimer_get();

  if (tracebuffer_index == BUFSIZE)
    tracebuffer_flush();
}

void
tracebuffer_finish (void)
{
  if (tracebuffer_index)
    tracebuffer_flush();
}
