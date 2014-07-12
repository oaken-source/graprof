
/******************************************************************************
 *                                  Grapes                                    *
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


#include "feedback.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "util.h"

extern const char *program_invocation_short_name;

void
feedback_error_at_line (const char *filename, unsigned int linenum, const char *format, ...)
{
  int errnum = errno;
  fprintf(stderr, "%s:%s:%u: error: ", program_invocation_short_name, filename, linenum);

  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  if(errnum)
    fprintf(stderr, ": %s", strerror(errnum));

  fprintf(stderr, "\n");

  errno = errnum;
}

void
feedback_error (int status, const char *format, ...)
{
  int errnum = errno;

  fprintf(stderr, "%s: error: ", program_invocation_short_name);

  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  if(errnum)
    fprintf(stderr, ": %s", strerror(errnum));

  fprintf(stderr, "\n");

  if(status != EXIT_SUCCESS)
    exit(status);

  errno = 0;
}

void
feedback_warning (const char *format, ...)
{
  int errnum = errno;

  fprintf(stderr, "%s: warning: ", program_invocation_short_name);

  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  if(errnum)
    fprintf(stderr, ": %s", strerror(errnum));

  fprintf(stderr, "\n");

  errno = 0;
}

