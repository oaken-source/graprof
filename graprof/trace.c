
#include "trace.h"

#include "function.h"
#include "memory.h"
#include "addr.h"

#include <stdio.h>

#include <grapes/feedback.h>

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

  int res = function_exit(time);
  assert_inner(!res, "function_exit");

  return 0;
}

static int
trace_malloc (void *buf)
{
  size_t size = buffer_get(size_t);
  uintptr_t caller = buffer_get(uintptr_t);
  uintptr_t result = buffer_get(uintptr_t);
  unsigned long long time = buffer_get(unsigned long long);

  printf("+ 0x%lu\n", caller);
 
  char *name;
  char *file;
  unsigned int line;

  addr_translate(caller, &name, &file, &line);

  printf("%s\n%s:%u\n", name, file, line);

  int res = memory_malloc(size, caller, result, time);
  assert_inner(!res, "memory_malloc");

  return 0;
}

static int
trace_realloc (void *buf)
{
  printf("*\n");
 
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
  printf("-\n");
 
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
trace_read (const char *filename)
{
  FILE *trace = fopen(filename, "r");
  assert_inner(trace, "fopen");

  void *trace_buf = NULL;
  unsigned long trace_bufsize = 0;

  size_t n = fread(&trace_bufsize, sizeof(unsigned long), 1, trace);
  assert_set_errno(ENOTSUP, n == 1, "fread");

  trace_buf = malloc(trace_bufsize);
  assert_inner(trace_buf, "malloc");

  n = fread(trace_buf, 1, trace_bufsize, trace);
  assert_set_errno(ENOTSUP, n == trace_bufsize, "fread");
  assert_set_errno(ENOTSUP, feof(trace), "feof");

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
          assert_set_errno(ENOTSUP, trace_bufsize == trace_index, "END not at end");
          return 0;
        default:
          assert_set_errno(ENOTSUP, 0, "sign switch");
          break;
        }
    }

  assert_set_errno(ENOTSUP, 0, "no END at end");

  free(trace_buf);
  fclose(trace);

  return 0;
}

unsigned long long
trace_get_total_runtime ()
{
  return trace_total_runtime;
}
