
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


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "feedback.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "util.h"

#ifndef _GNU_SOURCE
  extern const char *program_invocation_name;
#endif

void
feedback_error_at_line (const char *filename, unsigned int linenum, const char *format, ...)
{
  int errnum = errno;
    fprintf(stderr, "%s:%s:%u: ", program_invocation_name, filename, linenum);
  
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  if(errnum)
    fprintf(stderr, ": %s", strerror(errnum));

  fprintf(stderr, "\n");

  errno = errnum;
}

static char *feedback_error_prefix = NULL;

int
feedback_set_error_prefix (const char *format, ...)
{
  free(feedback_error_prefix);

  if (format == NULL)
    {
      feedback_error_prefix = NULL;
      return 0;
    }

  va_list args;
  va_start(args, format);
  int length = vsnprintf(NULL, 0, format, args);
  va_end(args);

  feedback_error_prefix = malloc(sizeof(*feedback_error_prefix) * (length + 1));
  assert_inner(feedback_error_prefix, "malloc");

  va_start(args, format);
  vsnprintf(feedback_error_prefix, length + 1, format, args);
  va_end(args);

  return 0;
}

void 
feedback_error (int status, const char *format, ...)
{
  int errnum = errno;

  if (feedback_error_prefix)
    fprintf(stderr, "%s", feedback_error_prefix);
  
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

static char *feedback_warning_prefix = NULL;

int
feedback_set_warning_prefix (const char *format, ...)
{
  free(feedback_warning_prefix);

  if (format == NULL)
    {
      feedback_warning_prefix = NULL;
      return 0;
    }

  va_list args;
  va_start(args, format);
  int length = vsnprintf(NULL, 0, format, args);
  va_end(args);

  feedback_warning_prefix = malloc(sizeof(*feedback_warning_prefix) * (length + 1));
  assert_inner(feedback_warning_prefix, "malloc");

  va_start(args, format);
  vsnprintf(feedback_warning_prefix, length + 1, format, args);
  va_end(args);

  return 0;
}

void 
feedback_warning (const char *format, ...)
{	
  int errnum = errno;

  if (feedback_warning_prefix)
    fprintf(stderr, "%s", feedback_warning_prefix);

  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);

  if(errnum)
    fprintf(stderr, ": %s", strerror(errnum));

  fprintf(stderr, "\n");

  errno = 0;
}

struct feedback_state
{
  int running;
  unsigned long max;
  unsigned long current;
  unsigned int progress;
  const char *tag;
};

static struct feedback_state state = { 0, 0, 0, 0, 0 };

char spinner[] = { '\\', '|', '/', '-' };

void 
feedback_progress_out ()
{
  if(state.max)
    printf("\r %3i %% %s", state.progress, state.tag);
  else
    printf("\r [ %c ] %s", spinner[state.progress], state.tag);
  fflush(stdout);
}

void 
feedback_progress_start (const char *tag, unsigned long max)
{
  if(state.running)
    feedback_progress_finish();

  state.running = 1;
  state.max = max;
  state.current = 0;
  state.progress = 0;
  state.tag = tag;

  feedback_progress_out();
}

void 
feedback_progress_inc ()
{
  if(state.running)
    {
      ++(state.current);
      if(state.max)
        {
          if(100.0 * state.current / state.max > state.progress + 1)
            {
              state.progress = 100.0 * state.current / state.max;
              feedback_progress_out();
            }
        }
      else
        {
          time_t now = time(NULL);
          if(now % 4 != state.progress)
            {
              state.progress = now % 4;
              feedback_progress_out();
            }
        }
    }
}

void 
feedback_progress_finish ()
{
  if(state.running)
    printf("\r [ " GREEN "*" NORMAL " ] %s\n", state.tag);
  state.running = 0;
}

#define CHUNK 20

char*
feedback_readline (const char *prompt)
{
  if(prompt)
    {
      printf("%s : ", prompt);
      fflush(stdout);
    }

  char *buf = malloc(sizeof(*buf) * CHUNK);
  assert_inner_ptr(buf, "malloc");

  unsigned int chars = 0;
  unsigned int chunks = 0;

  int c = getchar();
  while(c != EOF && c != '\n')
    {
      buf[chars] = c;
      ++chars;
      if(chars >= (chunks + 1) * CHUNK)
        {
          ++chunks;
          buf = realloc(buf, sizeof(*buf) * CHUNK * (chunks + 1));
          assert_inner_ptr(buf, "realloc");
        }

      c = getchar();
    }

  buf[chars + chunks * CHUNK] = 0;

  return buf;
}


