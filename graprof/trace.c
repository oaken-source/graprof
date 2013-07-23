
#include "trace.h"

//#include "timeline.h"
#include "function.h"

#include <stdio.h>

#include <grapes/feedback.h>

static unsigned long long trace_total_runtime = 0;

static int trace_enter(void *ptr);
static int trace_exit(void *ptr);
//static int trace_malloc(void *ptr);
//static int trace_realloc(void *ptr);
//static int trace_free(void *ptr);
static int trace_end(void *ptr);

static int
trace_enter (void *ptr)
{
  uintptr_t func;
  uintptr_t caller;
  unsigned long long time;

  func = *((uintptr_t*)(ptr));
  caller = *((uintptr_t*)(ptr + sizeof(uintptr_t)));
  time = *((unsigned long long*)(ptr + 2 * sizeof(uintptr_t)));

  //timeline_event *e = timeline_push_event(TIMELINE_EVENT_ENTER, time, caller);
  //assert_inner(e, "timeline_push_event");

  //e->func = func;

  int res = function_enter(func, caller, time);
  assert_inner(!res, "function_enter");

  return 0;
}

static int
trace_exit (void *ptr)
{
  unsigned long long time;

  time = *((unsigned long long*)(ptr)); 

  //timeline_event *e = timeline_push_event(TIMELINE_EVENT_EXIT, time, 0);
  //assert_inner(e, "timeline_push_event");

  int res = function_exit(0, time);
  assert_inner(!res, "function_exit");

  return 0;
}

/*static int
trace_malloc (void *ptr)
{
  size_t size;
  uintptr_t caller;
  uintptr_t result;
  unsigned long long time;

  int res = sscanf(line, "+ %zu 0x%" SCNxPTR " 0x%" SCNxPTR " %llu", &size, &caller, &result, &time);
  assert_inner(res == 4, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_MALLOC, time, caller);
  assert_inner(e, "timeline_push_event");

  e->size = size;
  e->result = result;

  return 0;
}

static int
trace_realloc (void *ptr)
{
  uintptr_t ptr;
  size_t size;
  uintptr_t caller;
  uintptr_t result;
  unsigned long long time;

  int res = sscanf(line, "* 0x%" SCNxPTR " %zu 0x%" SCNxPTR " 0x%" SCNxPTR " %llu", &ptr, &size, &caller, &result, &time);
  assert_inner(res == 5, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_REALLOC, time, caller);
  assert_inner(e, "timeline_push_event");

  e->size = size;
  e->result = result;
  e->ptr = ptr;

  return 0;
}

static int
trace_free (void *ptr)
{
  uintptr_t ptr;
  uintptr_t caller;
  unsigned long long time;

  int res = sscanf(line, "- 0x%" SCNxPTR " 0x%" SCNxPTR " %llu", &ptr, &caller, &time);
  assert_inner(res == 3, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_FREE, time, caller);
  assert_inner(e, "timeline_push_event");

  e->ptr = ptr;

  return 0;
}*/

static int
trace_end (void *ptr)
{
  unsigned long long time;

  time = *((unsigned long long*)(ptr));

  //timeline_event *e = timeline_push_event(TIMELINE_EVENT_END, time, 0);
  //assert_inner(e, "timeline_push_event");

  //timeline_finalize();

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
          assert_set_errno(ENOSYS, 0, "+");
          break;
        case '*':
          assert_set_errno(ENOSYS, 0, "*");
          break;
        case '-':
          assert_set_errno(ENOSYS, 0, "-");
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
