
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


#include "trace.h"

#include "function.h"
#include "memory.h"
#include "addr.h"

#include <stdio.h>
#include <string.h>

#include <grapes/feedback.h>

#if HAVE_OPENSSL_MD5
  #include <openssl/md5.h>
#elif HAVE_BSD_MD5
  #include <bsd/md5.h>
#endif

#define buffer_get(T) *((T*)(buf)); buf += sizeof(T)

static unsigned long long trace_total_runtime = 0;

static int trace_enter(void *buf);
static int trace_exit(void *buf);
static int trace_malloc(void *buf);
static int trace_realloc(void *buf);
static int trace_free(void *buf);
static int trace_end(void *buf);

static int
trace_enter (void *buf)
{
  uintptr_t func = buffer_get(uintptr_t);
  uintptr_t caller = buffer_get(uintptr_t);
  unsigned long long time = buffer_get(unsigned long long);

  int res = function_enter(func, caller, time);
  assert_inner(!res, "function_enter");

  return 0;
}

static int
trace_exit (void *buf)
{
  unsigned long long time = buffer_get(unsigned long long);
  function_exit(time);

  return 0;
}

static int
trace_malloc (void *buf)
{
  size_t size = buffer_get(size_t);
  uintptr_t caller = buffer_get(uintptr_t);
  uintptr_t result = buffer_get(uintptr_t);
  unsigned long long time = buffer_get(unsigned long long);

  int res = memory_malloc(size, caller, result, time);
  assert_inner(!res, "memory_malloc");

  return 0;
}

static int
trace_realloc (void *buf)
{
  uintptr_t ptr = buffer_get(uintptr_t);
  size_t size = buffer_get(size_t);
  uintptr_t caller = buffer_get(uintptr_t);
  uintptr_t result = buffer_get(uintptr_t);
  unsigned long long time = buffer_get(unsigned long long);

  int res = memory_realloc(ptr, size, caller, result, time);
  assert_inner(!res, "memory_realloc");

  return 0;
}

static int
trace_free (void *buf)
{
  uintptr_t ptr = buffer_get(uintptr_t);
  uintptr_t caller = buffer_get(uintptr_t);
  unsigned long long time = buffer_get(unsigned long long);

  int res = memory_free(ptr, caller, time);
  assert_inner(!res, "memory_free");

  return 0;
}

static int
trace_end (void *buf)
{
  unsigned long long time = buffer_get(unsigned long long);

  int res = function_exit_all(time);
  assert_inner(!res, "function_exit_all");

  trace_total_runtime = time;

  return 0;
}

int
trace_read (const char *filename, unsigned char md5_binary[16])
{
  FILE *trace = fopen(filename, "r");
  assert_inner(trace, "fopen");

  unsigned char md5[DIGEST_LENGTH] = { 0 };
  void *trace_buf = NULL;
  unsigned long trace_bufsize = 0;

  size_t n = fread(md5, 1, DIGEST_LENGTH, trace);
  assert_set_errno(ENOTSUP, n == DIGEST_LENGTH, "fread");

  unsigned char md5_zero[DIGEST_LENGTH] = { 0 };

  feedback_assert(!memcmp(md5, md5_zero, DIGEST_LENGTH) || !memcmp(md5, md5_binary, DIGEST_LENGTH), "digest verification failed. This trace was not generated with this executable!"); 

  n = fread(&trace_bufsize, sizeof(unsigned long), 1, trace);
  assert_set_errno(ENOTSUP, n == 1, "fread");

  trace_buf = malloc(trace_bufsize);
  assert_inner(trace_buf, "malloc");

  n = fread(trace_buf, 1, trace_bufsize, trace);
  assert_set_errno(ENOTSUP, n == trace_bufsize, "fread");
  assert_set_errno(ENOTSUP, feof(trace), "feof");

  unsigned int trace_ended = 0;

  unsigned long trace_index = 0;
  while (trace_index < trace_bufsize)
    {
      char sign = *((char*)(trace_buf + trace_index));
      trace_index += sizeof(char);
      switch (sign)
        {
        case 'e':
          trace_enter(trace_buf + trace_index);
          trace_index += 2 * sizeof(uintptr_t) + sizeof(unsigned long long);
          break;
        case 'x':
          trace_exit(trace_buf + trace_index);
          trace_index += sizeof(unsigned long long);
          break;
        case '+':
          trace_malloc(trace_buf + trace_index);
          trace_index += sizeof(size_t) + 2 * sizeof(uintptr_t) + sizeof(unsigned long long);
          break;
        case '*':
          trace_realloc(trace_buf + trace_index);
          trace_index += sizeof(size_t) + 3 * sizeof(uintptr_t) + sizeof(unsigned long long);
          break;
        case '-':
          trace_free(trace_buf + trace_index);
          trace_index += 2 * sizeof(uintptr_t) + sizeof(unsigned long long);
          break;
        case 'E':
          trace_end(trace_buf + trace_index);
          trace_index += sizeof(unsigned long long);
          trace_ended = 1;
          assert_set_errno(ENOTSUP, trace_bufsize == trace_index, "END not at end");
          break;
        default:
          assert_set_errno(ENOTSUP, 0, "unrerognized entry signs character '%c'", sign);
          break;
        }
    }

  if (!trace_ended)
    assert_set_errno(ENOTSUP, 0, "no END at end");

  free(trace_buf);
  fclose(trace);

  return 0;
}

unsigned long long
trace_get_total_runtime (void)
{
  return trace_total_runtime;
}
