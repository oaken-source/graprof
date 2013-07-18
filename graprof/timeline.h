
#pragma once

#include <inttypes.h>
#include <stdlib.h>

#define TIMELINE_EVENT_ENTER      0x01
#define TIMELINE_EVENT_EXIT       0x02
#define TIMELINE_EVENT_MALLOC     0x03
#define TIMELINE_EVENT_REALLOC    0x04
#define TIMELINE_EVENT_FREE       0x05
#define TIMELINE_EVENT_END        0x06

struct timeline_event
{
  unsigned int type;
  unsigned long long time;
  uintptr_t caller;

  union
  {
    uintptr_t func;

    struct
    {
      uintptr_t ptr;
      size_t size;
      uintptr_t result;
    };
  };
};

typedef struct timeline_event timeline_event;

timeline_event* timeline_push_event(unsigned int, unsigned long long, uintptr_t);

timeline_event* timeline_head();

void timeline_sort();
