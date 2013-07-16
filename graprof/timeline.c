
#include "timeline.h"

#include <stdlib.h>

#include <grapes/util.h>

timeline_event *events = NULL;
unsigned int nevents = 0;

timeline_event* 
timeline_push_event (unsigned int type, unsigned long time, unsigned long caller)
{
  ++nevents;
  events = realloc(events, sizeof(timeline_event) * nevents);
  assert_inner_ptr(events, "realloc");

  timeline_event *e = events + nevents - 1;
  e->type = type;
  e->time = time;
  e->caller = caller;

  return e;
}

