
#include "strtime.h"

#define MAX_TIMEVAL 100000

void 
strtime (unsigned long long *time, const char **prefix)
{
  *prefix = "n";
  if (*time >= MAX_TIMEVAL)
    {
      *prefix = "µ";
      *time /= 1000;
    }

  if (*time >= MAX_TIMEVAL)
    {
      *prefix = "m";
      *time /= 1000;
    }

  if (*time >= MAX_TIMEVAL)
    {
      *prefix = " ";
      *time /= 1000;
    }
}

