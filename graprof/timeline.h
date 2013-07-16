
#pragma once

#define TIMELINE_EVENT_ENTER      0x01
#define TIMELINE_EVENT_EXIT       0x02
#define TIMELINE_EVENT_MALLOC     0x03
#define TIMELINE_EVENT_REALLOC    0x04
#define TIMELINE_EVENT_FREE       0x05

struct timeline_event
{
  unsigned int type;
  unsigned long time;
  unsigned long caller;

  union
  {
    unsigned long func;

    struct
    {
      unsigned long ptr;
      unsigned int size;
      unsigned long result;
    };
  };
};

typedef struct timeline_event timeline_event;

timeline_event* timeline_push_event(unsigned int, unsigned long, unsigned long);
