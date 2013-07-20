
#include "timeline.h"

#include <stdio.h>
#include <stdlib.h>

#include <grapes/util.h>

static timeline_event *events = NULL;
static unsigned int nevents = 0;

static unsigned long long timeline_total_runtime = 0;

timeline_event* 
timeline_push_event (unsigned int type, unsigned long long time, uintptr_t caller)
{
  ++nevents;
  events = realloc(events, sizeof(*events) * nevents);
  assert_inner_ptr(events, "realloc");

  timeline_event *e = events + nevents - 1;
  e->type = type;
  e->time = time;
  e->caller = caller;

  return e;
}

timeline_event*
timeline_head ()
{
  return events;
}

void
timeline_finalize ()
{
  timeline_total_runtime = events[nevents - 1].time;
}

unsigned long long 
timeline_get_total_runtime ()
{
  return timeline_total_runtime;
}
