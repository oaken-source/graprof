
#include "timeline.h"

#include <stdio.h>
#include <stdlib.h>

#include <grapes/util.h>

timeline_event *events = NULL;
unsigned int nevents = 0;

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

static int
timeline_compar (const void *p1, const void *p2)
{
  const timeline_event *e1 = p1;
  const timeline_event *e2 = p2;

  return e1->time - e2->time;
}

void
timeline_sort ()
{
  qsort(events, nevents, sizeof(*events), timeline_compar);
}
