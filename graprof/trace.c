
#include "trace.h"

#include "timeline.h"

#include <stdio.h>

#include <grapes/feedback.h>

int 
trace_enter (const char *line)
{
  unsigned long func;
  unsigned long caller;
  unsigned long time;

  int res = sscanf(line, "e 0x%lx 0x%lx %lu", &func, &caller, &time);
  assert_inner(res == 3, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_ENTER, time, caller);
  assert_inner(e, "timeline_push_event");

  e->func = func;

  return 0;
}

int
trace_exit (const char *line)
{
  unsigned long func;
  unsigned long caller;
  unsigned long time;

  unsigned int res = sscanf(line, "x 0x%lx 0x%lx %lu", &func, &caller, &time);
  assert_inner(res == 3, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_EXIT, time, caller);
  assert_inner(e, "timeline_push_event");

  e->func = func;

  return 0;
}

int
trace_malloc (const char *line)
{
  unsigned int size;
  unsigned long caller;
  unsigned long result;
  unsigned long time;

  unsigned int res = sscanf(line, "+ %u 0x%lx 0x%lx %lu", &size, &caller, &result, &time);
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
  unsigned long ptr;
  unsigned int size;
  unsigned long caller;
  unsigned long result;
  unsigned long time;

  unsigned int res = sscanf(line, "* 0x%lx %u 0x%lx 0x%lx %lu", &ptr, &size, &caller, &result, &time);
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
  unsigned long ptr;
  unsigned long caller;
  unsigned long time;

  unsigned int res = sscanf(line, "- 0x%lx 0x%lx %lu", &ptr, &caller, &time);
  assert_inner(res == 3, "sscanf");

  timeline_event *e = timeline_push_event(TIMELINE_EVENT_FREE, time, caller);
  assert_inner(e, "timeline_push_event");

  e->ptr = ptr;

  return 0;
}
