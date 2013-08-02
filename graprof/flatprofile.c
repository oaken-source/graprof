
#include "flatprofile.h"

#include "function.h"
#include "trace.h"
#include "strtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

extern FILE *graprof_out;

static int
cmpfunction (const void *p1, const void *p2)
{
  function *f1 = *(function**)p1;
  function *f2 = *(function**)p2;

  if (f2->self_time == f1->self_time)
    return strcmp(f1->name, f2->name);

  if (f2->self_time < f1->self_time)
    return -1;
  return 1;
}

int 
flatprofile_print ()
{
  fprintf(graprof_out, "Flat profile:\n");
  fprintf(graprof_out, "\n");

  const char *prefix;
  unsigned long long time = trace_get_total_runtime();
 
  strtime(&time, &prefix);

  fprintf(graprof_out, " total runtime: %llu %sseconds\n", time, prefix);
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, "  %%       self    children             self     total\n");
  fprintf(graprof_out, " time      time      time     calls    /call     /call  name\n");
 
  unsigned int nfunctions = 0;
  function *functions = function_get_all(&nfunctions);

  function **sorted_functions = malloc(sizeof(function*) * nfunctions);
  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
    sorted_functions[i] = functions + i;

  qsort(sorted_functions, nfunctions, sizeof(function*), cmpfunction);

  for (i = 0; i < nfunctions; ++i)
    {
      function *f = sorted_functions[i];

      time = trace_get_total_runtime();
      fprintf(graprof_out, "%6.2f ", (100.0 * f->self_time) / time);

      time = f->self_time;
      strtime(&time, &prefix);

      fprintf(graprof_out, "%6llu %ss ", time, prefix);

      time = f->children_time;
      strtime(&time, &prefix);

      fprintf(graprof_out, "%6llu %ss ", time, prefix);
  
      fprintf(graprof_out, "%8lu ", f->calls);

      time = f->self_time / f->calls;
      strtime(&time, &prefix);

      fprintf(graprof_out, "%6llu %ss ", time, prefix);

      time = f->children_time / f->calls;
      strtime(&time, &prefix);

      fprintf(graprof_out, "%6llu %ss  ", time, prefix);

      if (!strcmp(f->name, "??"))
        fprintf(graprof_out, "0x%" PRIxPTR , f->address);
      else
        fprintf(graprof_out, "%s", f->name);
      fprintf(graprof_out, " [%tu]\n", f - functions);
    }

  free(sorted_functions);

  fprintf(graprof_out, "\n");
  fprintf(graprof_out, "  %%         the percentage of the total running time of the\n"); 
  fprintf(graprof_out, " time       program spent in this function\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " self       the absolute total running time spent in this\n");
  fprintf(graprof_out, "  time      function alone - this is the major sort of this\n");
  fprintf(graprof_out, "            listing\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " children   the absolute total running time of the program\n");
  fprintf(graprof_out, "    time    spent in the descendants of this function in the\n");
  fprintf(graprof_out, "            call tree\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " calls      the number of times this function was invoked\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " self       the average running time spent in this function\n");
  fprintf(graprof_out, " /call      per call\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " total      the average running time spent in the descendants\n");
  fprintf(graprof_out, "            of this function in the call tree per call\n");
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " name       the name of the function, if available, else its\n");
  fprintf(graprof_out, "            address, and a unique id for reference in the call\n");
  fprintf(graprof_out, "            graph - this is the minor sort of this listing\n");
  fprintf(graprof_out, "\n");

  return 0;
}
