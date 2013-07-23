
#include "profileout.h"

#include "timeline.h"
#include "function.h"

#include <stdio.h>
#include <string.h>

extern FILE *graprof_out;

#define MAX_TIMEVAL 100000

static void 
profileout_format_time (unsigned long long *time, const char **prefix)
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

int 
profileout_flat_profile ()
{
  fprintf(graprof_out, "Flat profile:\n");
  fprintf(graprof_out, "\n");

  const char *prefix;
  unsigned long long time = timeline_get_total_runtime();
 
  profileout_format_time(&time, &prefix);

  fprintf(graprof_out, " total runtime: %llu %sseconds\n", time, prefix);
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, "  %%    cumulative   self               self     total\n");
  fprintf(graprof_out, " time      time      time     calls    /call     /call   name\n");
 
  unsigned int nfunctions = 0;
  function *functions = function_get_all(&nfunctions);

  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
  {
    function *f = functions + i;

    fprintf(graprof_out, "%6.2f ", (100.0 * f->self_time) / timeline_get_total_runtime());

    time = f->cumulative_time;
    profileout_format_time(&time, &prefix);

    fprintf(graprof_out, "%6llu %ss ", time, prefix);

    time = f->self_time;
    profileout_format_time(&time, &prefix);

    fprintf(graprof_out, "%6llu %ss ", time, prefix);

    fprintf(graprof_out, "%8lu ", f->calls);

    time = f->self_time / f->calls;
    profileout_format_time(&time, &prefix);

    fprintf(graprof_out, "%6llu %ss ", time, prefix);

    time = f->cumulative_time / f->calls;
    profileout_format_time(&time, &prefix);

    fprintf(graprof_out, "%6llu %ss  ", time, prefix);

    fprintf(graprof_out, "%s", f->name);
    if (!strcmp(f->name, "??"))
      fprintf(graprof_out, " [0x%" PRIxPTR "]", f->address);
    fprintf(graprof_out, "\n");
  }

  fprintf(graprof_out, "\n");
  fprintf(graprof_out, "  %%         the percentage of the total running time of the\n"); 
  fprintf(graprof_out, " time       program spent in this function\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " cumulative the absolute total running time of the program\n");
  fprintf(graprof_out, "     time   spent below this function in the call tree\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " self       the absolute total running time spent in this\n");
  fprintf(graprof_out, "  time      function alone\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " calls      the number of times this function was invoked\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " self       the average running time spent in this function\n");
  fprintf(graprof_out, " /call      per call\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " total      the average running time spent below this function\n");
  fprintf(graprof_out, "  /call     in the call tree per call\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " name       the name of the function, if available, else its\n");
  fprintf(graprof_out, "            address\n");

  return 0;
}
