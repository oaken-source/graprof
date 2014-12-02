
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

#include <grapes/feedback.h>
#include <grapes/file.h>

#include <stdio.h>
#include <string.h>

static unsigned long long trace_total_runtime = 0;

static int
trace_init (tracebuffer_packet *p, unsigned char md5_binary[DIGEST_LENGTH])
{
  unsigned char *md5 =      p->init.digest;

  feedback_assert_wrn(!memcmp(md5, md5_binary, DIGEST_LENGTH), "binary digest verification failed");

  return 0;
}

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

int
trace_read (const char *filename, unsigned char md5_binary[DIGEST_LENGTH])
{
  size_t length;
  tracebuffer_packet *packets = file_map(filename, &length);
  assert_inner(packets, "file_map");

  unsigned int trace_ended = 0;

  const size_t npackets = length / sizeof(*packets);
  size_t i;
  for (i = 0; i < npackets; ++i)
    {
      assert_set_errno(!trace_ended, ENOTSUP, "packets after END");
      tracebuffer_packet *p = packets + i;

      switch (p->type)
        {
        case 'I':
          __checked_call(0 == trace_init(p, md5_binary));
          break;
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
          assert_set_errno(0, ENOTSUP, "invalid tracebuffer type '%c'", p->type);
          break;
        }
    }

  if (!trace_ended)
    {
      feedback_warning("libgraprof trace not properly terminated.");
      __checked_call(0 == trace_end(packets + npackets - 1));
    }

  __checked_call(0 == file_unmap(packets, length));

  return 0;
}

unsigned long long
trace_get_total_runtime (void)
{
  return trace_total_runtime;
}
