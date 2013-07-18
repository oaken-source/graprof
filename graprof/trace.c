
#include "trace.h"

#include "timeline.h"

#include <stdio.h>

#include <grapes/feedback.h>

int 
trace_enter (const char *line)
{
  uintptr_t func;
  uintptr_t caller;
  unsigned long long time;

  int res = sscanf(line, "e 0x%" SCNxPTR " 0x%" SCNxPTR " %llu", &func, &caller, &time);
  assert_inner(res == 3, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_ENTER, time, caller);
  assert_inner(e, "timeline_push_event");

  e->func = func;

  return 0;
}

int
trace_exit (const char *line)
{
  uintptr_t func;
  uintptr_t caller;
  unsigned long long time;

  unsigned int res = sscanf(line, "x 0x%" SCNxPTR " 0x%" SCNxPTR " %llu", &func, &caller, &time);
  assert_inner(res == 3, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_EXIT, time, caller);
  assert_inner(e, "timeline_push_event");

  e->func = func;

  return 0;
}

int
trace_malloc (const char *line)
{
  size_t size;
  uintptr_t caller;
  uintptr_t result;
  unsigned long long time;

  unsigned int res = sscanf(line, "+ %zu 0x%" SCNxPTR " 0x%" SCNxPTR " %llu", &size, &caller, &result, &time);
  assert_inner(res == 4, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_MALLOC, time, caller);
  assert_inner(e, "timeline_push_event");

  e->size = size;
  e->result = result;

  return 0;
}

int
trace_realloc (const char *line)
{
  uintptr_t ptr;
  size_t size;
  uintptr_t caller;
  uintptr_t result;
  unsigned long long time;

  unsigned int res = sscanf(line, "* 0x%" SCNxPTR " %zu 0x%" SCNxPTR " 0x%" SCNxPTR " %llu", &ptr, &size, &caller, &result, &time);
  assert_inner(res == 5, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_REALLOC, time, caller);
  assert_inner(e, "timeline_push_event");

  e->size = size;
  e->result = result;
  e->ptr = ptr;

  return 0;
}

int
trace_free (const char *line)
{
  uintptr_t ptr;
  uintptr_t caller;
  unsigned long long time;

  unsigned int res = sscanf(line, "- 0x%" SCNxPTR " 0x%" SCNxPTR " %llu", &ptr, &caller, &time);
  assert_inner(res == 3, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_FREE, time, caller);
  assert_inner(e, "timeline_push_event");

  e->ptr = ptr;

  return 0;
}

int
trace_end (const char *line)
{
  unsigned long long time;

  unsigned int res = sscanf(line, "END %llu", &time);
  assert_inner(res == 1, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_END, time, 0);
  assert_inner(e, "timeline_push_event");

  timeline_sort();

  return 0;
}
