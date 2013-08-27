
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013  Andreas Grapentin                                   *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ******************************************************************************/


#include "callgraph.h"

#include "trace.h"
#include "strtime.h"
#include "function.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

extern FILE *graprof_out;

void
callgraph_print ()
{
  fprintf(graprof_out, "Call graph:\n");
  fprintf(graprof_out, "\n");

  const char *prefix;
  unsigned long long time = trace_get_total_runtime();
 
  strtime(&time, &prefix);

  fprintf(graprof_out, " total runtime:                  %llu %sseconds\n", time, prefix);
  fprintf(graprof_out, " total number of function calls: %llu\n", function_get_total_calls());
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
      unsigned int j;
      for (j = 0; j < f->ncallers; ++j)
        {
          if (f->callers[j].function_id == (unsigned int)-1)
            {
              fprintf(graprof_out, "                                   %8lu/%-8lu      <spontaneous>\n", f->callers[j].calls, f->calls);
            }
          else
            {
              time = f->callers[j].self_time;
              strtime(&time, &prefix);
              fprintf(graprof_out, "               %6llu %ss ", time, prefix);

              time = f->callers[j].children_time;
              strtime(&time, &prefix);
              fprintf(graprof_out, "%6llu %ss ", time, prefix);

              fprintf(graprof_out, "%8lu/%-8lu  ", f->callers[j].calls, f->calls);

              if (!strcmp(functions[f->callers[j].function_id].name, "??"))
                fprintf(graprof_out, "    0x%" PRIxPTR , functions[f->callers[j].function_id].address);
              else
                fprintf(graprof_out, "    %s", functions[f->callers[j].function_id].name);
              fprintf(graprof_out, " [%u]\n", f->callers[j].function_id);
            }
        }

      // print self
      time = trace_get_total_runtime();
      fprintf(graprof_out, " %6u %6.2f ", i, (100.0 * (f->self_time + f->children_time)) / time);
 
      time = f->self_time;
      strtime(&time, &prefix);
      fprintf(graprof_out, "%6llu %ss ", time, prefix);

      time = f->children_time;
      strtime(&time, &prefix);
      fprintf(graprof_out, "%6llu %ss ", time, prefix);

      fprintf(graprof_out, "%8lu           ", f->calls);

      if (!strcmp(f->name, "??"))
        fprintf(graprof_out, "0x%" PRIxPTR , f->address);
      else
        fprintf(graprof_out, "%s", f->name);
      fprintf(graprof_out, " [%u]\n", i);

      // print children
      for (j = 0; j < f->ncallees; ++j)
        {
          time = f->callees[j].self_time;
          strtime(&time, &prefix);
          fprintf(graprof_out, "               %6llu %ss ", time, prefix);

          time = f->callees[j].children_time;
          strtime(&time, &prefix);
          fprintf(graprof_out, "%6llu %ss ", time, prefix);

          fprintf(graprof_out, "%8lu/%-8lu  ", f->callees[j].calls, functions[f->callees[j].function_id].calls);

          if (!strcmp(functions[f->callees[j].function_id].name, "??"))
            fprintf(graprof_out, "    0x%" PRIxPTR , functions[f->callees[j].function_id].address);
          else
            fprintf(graprof_out, "    %s", functions[f->callees[j].function_id].name);
          fprintf(graprof_out, " [%u]\n", f->callees[j].function_id);
        }

      fprintf(graprof_out, " -------------------------------------------------------\n");
    }

  fprintf(graprof_out, "\n");
}
