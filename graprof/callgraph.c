
#include "callgraph.h"

#include "trace.h"
#include "strtime.h"
#include "function.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>

extern FILE *graprof_out;

int
callgraph_print ()
{
  fprintf(graprof_out, "Call graph:\n");
  fprintf(graprof_out, "\n");

  const char *prefix;
  unsigned long long time = trace_get_total_runtime();
 
  strtime(&time, &prefix);

  fprintf(graprof_out, " total runtime: %llu %sseconds\n", time, prefix);
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, "          %%       self     children\n");
  fprintf(graprof_out, "  index  time      time      time       called       name\n");

  unsigned int nfunctions = 0;
  function *functions = function_get_all(&nfunctions);

  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
    {
      function *f = functions + i;

      // print parents

      // print self
      time = trace_get_total_runtime();
      fprintf(graprof_out, " %6u %6.2f ", i, (100.0 * f->self_time) / time);
 
      time = f->self_time;
      strtime(&time, &prefix);
      fprintf(graprof_out, "%6llu %ss ", time, prefix);

      time = f->cumulative_time;
      strtime(&time, &prefix);
      fprintf(graprof_out, "%6llu %ss ", time, prefix);

      fprintf(graprof_out, "%8lu           ", f->calls);
      // fprintf(graprof_out, "%8lu/%-8lu  ", f->calls, f->calls);

      if (!strcmp(f->name, "??"))
        fprintf(graprof_out, "0x%" PRIxPTR , f->address);
      else
        fprintf(graprof_out, "%s", f->name);
      fprintf(graprof_out, " [%u]\n", i);

      // print children
      
      fprintf(graprof_out, " -------------------------------------------------------\n");
    }
 
  return 0;
}
