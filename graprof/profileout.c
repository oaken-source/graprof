
#include "profileout.h"

#include "timeline.h"
#include "function.h"

#include <stdio.h>
#include <string.h>

extern FILE *graprof_out;

static void 
profileout_format_time (unsigned long long *time, const char **prefix)
{
  *prefix = "n";
  if (*time >= 1000000)
    {
      *prefix = "µ";
      *time /= 1000;
    }

  if (*time >= 1000000)
    {
      *prefix = "m";
      *time /= 1000;
    }

  if (*time >= 1000000)
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
  fprintf(graprof_out, "  %%    cumulative     self             self      total\n");
  fprintf(graprof_out, " time     time        time     calls    /call     /call   name\n");
 
  unsigned int nfunctions = 0;
  function *functions = function_get_all(&nfunctions);

  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
  {
    function *f = functions + i;

    fprintf(graprof_out, "%6.2f ", (1.0 * f->cumulative_time) / timeline_get_total_runtime());

    time = f->cumulative_time;
    profileout_format_time(&time, &prefix);

    fprintf(graprof_out, "%6llu %ss ", time, prefix);

    time = f->self_time;
    profileout_format_time(&time, &prefix);

    fprintf(graprof_out, "%8llu %ss ", time, prefix);

    fprintf(graprof_out, "%7lu ", f->calls);

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

  return 0;
}
