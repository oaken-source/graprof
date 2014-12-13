
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


#include "trace.h"

#include "function.h"
#include "memory.h"
#include "addr.h"

#include "libgraprof/tracebuffer.h"

#include <grapes/feedback.h>
#include <grapes/file.h>

#include <stdio.h>
#include <string.h>

static unsigned long long trace_total_runtime = 0;

static int
trace_enter (tracebuffer_packet *p)
{
  uintptr_t func =          p->enter.func;
  uintptr_t caller =        p->enter.caller;
  unsigned long long time = p->time;

  __checked_call(0 == function_enter(func, caller, time));

  return 0;
}

static int
trace_exit (tracebuffer_packet *p)
{
  unsigned long long time = p->time;
  function_exit(time);

  return 0;
}

static int
trace_malloc (tracebuffer_packet *p)
{
  size_t size =             p->malloc.size;
  uintptr_t caller =        p->malloc.caller;
  uintptr_t result =        p->malloc.result;
  unsigned long long time = p->time;

  __checked_call(0 == memory_malloc(size, caller, result, time));

  return 0;
}

static int
trace_realloc (tracebuffer_packet *p)
{
  uintptr_t ptr =           p->realloc.ptr;
  size_t size =             p->realloc.size;
  uintptr_t caller =        p->realloc.caller;
  uintptr_t result =        p->realloc.result;
  unsigned long long time = p->time;

  __checked_call(0 == memory_realloc(ptr, size, caller, result, time));

  return 0;
}

static int
trace_free (tracebuffer_packet *p)
{
  uintptr_t ptr =           p->free.ptr;
  uintptr_t caller =        p->free.caller;
  unsigned long long time = p->time;

  __checked_call(0 == memory_free(ptr, caller, time));

  return 0;
}

static int
trace_end (tracebuffer_packet *p)
{
  unsigned long long time = p->time;

  __checked_call(0 == function_exit_all(time));

  trace_total_runtime = time;

  return 0;
}

static int __may_fail
trace_iterate (tracebuffer_packet *packets, size_t length)
{
  unsigned int trace_ended = 0;

  const size_t npackets = length / sizeof(*packets);
  size_t i;
  for (i = 0; i < npackets; ++i)
    {
      __precondition(ENOTSUP, !trace_ended);
      tracebuffer_packet *p = packets + i;

      switch (p->type)
        {
        case 'e':
          __checked_call(0 == trace_enter(p));
          break;
        case 'x':
          __checked_call(0 == trace_exit(p));
          break;
        case '+':
          __checked_call(0 == trace_malloc(p));
          break;
        case '*':
          __checked_call(0 == trace_realloc(p));
          break;
        case '-':
          __checked_call(0 == trace_free(p));
          break;
        case 'E':
          __checked_call(0 == trace_end(p));
          trace_ended = 1;
          break;
        default:
          __precondition(ENOTSUP, 0 && p->type);
          break;
        }
    }

  if (!trace_ended)
    {
      feedback_warning("libgraprof trace not properly terminated.");
      __checked_call(0 == trace_end(packets + npackets - 1));
    }

  return 0;
}

static int __may_fail
trace_read_header (FILE *in)
{
  unsigned char magic[sizeof(TRACEBUFFER_MAGIC_NUMBER)];
  size_t res = fread(magic, 1, sizeof(TRACEBUFFER_MAGIC_NUMBER), in);
  __precondition(ENOTSUP, res == sizeof(TRACEBUFFER_MAGIC_NUMBER));

  __precondition(ENOTSUP, !memcmp(magic, TRACEBUFFER_MAGIC_NUMBER, sizeof(TRACEBUFFER_MAGIC_NUMBER)));

  uint32_t count;
  res = fread(&count, sizeof(count), 1, in);
  __precondition(ENOTSUP, res == 1);

  size_t i;
  for (i = 0; i < count; ++i)
    {
      uintptr_t offset;
      res = fread(&offset, sizeof(offset), 1, in);
      __precondition(ENOTSUP, res == 1);

      unsigned char digest[DIGEST_LENGTH];
      res = fread(digest, 1, DIGEST_LENGTH, in);
      __precondition(ENOTSUP, res == DIGEST_LENGTH);

      size_t length;
      res = fread(&length, sizeof(length), 1, in);
      __precondition(ENOTSUP, res == 1);

      char *filename;
      __checked_call(NULL != (filename = malloc(length)));
      res = fread(filename, 1, length, in);
      __checked_call(res == length,
        free(filename);
        errno = ENOTSUP;
      );

      __checked_call(0 == addr_extract_symbols(filename, offset),
        free(filename);
      );

      free(filename);
    }

  return ftell(in);
}

int
trace_read (const char *filename)
{
  FILE *in;
  __checked_call(NULL != (in = fopen(filename, "rb")));

  long offset;
  __checked_call(0 <= (offset = trace_read_header(in)),
    fclose(in);
  );

  fclose(in);

  size_t length;
  void *packets;
  __checked_call(NULL != (packets = file_map(filename, &length)));

  __checked_call(0 == trace_iterate(packets + offset, length - offset),
    __checked_call(0 == file_unmap(packets, length));
  );

  __checked_call(0 == file_unmap(packets, length));

  return 0;
}

unsigned long long
trace_get_total_runtime (void)
{
  return trace_total_runtime;
}
