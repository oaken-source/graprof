
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013 - 2014  Andreas Grapentin                            *
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

#define _(...) fprintf(graprof_out, __VA_ARGS__)

void
callgraph_print (int verbose)
{
  _("Call graph:\n");
  _("\n");

  const char *prefix;
  unsigned long long time = trace_get_total_runtime();

  prefix = strtime(&time);

  unsigned int nfunctions = function_get_nfunctions();

  _(" total runtime:                      %llu %sseconds\n", time, prefix);
  _(" total number of function calls:     %llu\n", function_get_total_calls());
  _(" total number of distinct functions: %u\n", nfunctions);
  _("\n");
  _("          %%           self     children\n");
  _("  index  time          time      time       called       name\n");

  function *functions = function_get_all();

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
              _("                                       %8lu/%-8lu      <spontaneous>\n", f->callers[j].calls, f->calls);
            }
          else
            {
              time = f->callers[j].self_time;
              prefix = strtime(&time);
              _("                   %6llu %ss ", time, prefix);

              time = f->callers[j].children_time;
              prefix = strtime(&time);
              _("%6llu %ss ", time, prefix);

              _("%8lu/%-8lu  ", f->callers[j].calls, f->calls);

              if (!strcmp(functions[f->callers[j].function_id].name, "??"))
                _("    0x%" PRIxPTR , functions[f->callers[j].function_id].address);
              else
                _("    %s", functions[f->callers[j].function_id].name);
              _(" [%u]\n", f->callers[j].function_id);
            }
        }

      // print self
      time = trace_get_total_runtime();
      _(" %6u %6.2f     ", i, (100.0 * (f->self_time + f->children_time)) / time);

      time = f->self_time;
      prefix = strtime(&time);
      _("%6llu %ss ", time, prefix);

      time = f->children_time;
      prefix = strtime(&time);
      _("%6llu %ss ", time, prefix);

      _("%8lu           ", f->calls);

      if (!strcmp(f->name, "??"))
        _("0x%" PRIxPTR , f->address);
      else
        _("%s", f->name);
      _(" [%u]\n", i);

      // print children
      for (j = 0; j < f->ncallees; ++j)
        {
          time = f->callees[j].self_time;
          prefix = strtime(&time);
          _("            %6.2f %6llu %ss ", (100.0 * (f->callees[j].self_time + f->callees[j].children_time)) / (f->self_time + f->children_time), time, prefix);

          time = f->callees[j].children_time;
          prefix = strtime(&time);
          _("%6llu %ss ", time, prefix);

          _("%8lu/%-8lu  ", f->callees[j].calls, functions[f->callees[j].function_id].calls);

          if (!strcmp(functions[f->callees[j].function_id].name, "??"))
            _("    0x%" PRIxPTR , functions[f->callees[j].function_id].address);
          else
            _("    %s", functions[f->callees[j].function_id].name);
          _(" [%u]\n", f->callees[j].function_id);
        }

      _(" -------------------------------------------------------\n");
    }

  _("\n");

  if (verbose >= 1)
    {
      _(" This table describes the call tree of the program, and was\n");
      _(" sorted by the total amount of time spent in and below each\n");
      _(" function in the call graph.\n");
      _("\n");
      _(" Each entry in this table consists of several lines. The line\n");
      _(" with the index number at the left hand margin lists the current\n");
      _(" function, and the lines above it lists the functions callers, and\n");
      _(" the functions below it list the functions callees.\n");
      _("\n");
      _(" The current function line lists:\n");
      _("   index      A unique function identifier that is also referenced\n");
      _("              in the name field of this listing.\n");
      _("\n");
      _("   %%          This is the percantage of the total time spent in and\n");
      _("   time       below this function in the call tree.\n");
      _("\n");
      _("   self       This is the total amount of time spent in this function.\n");
      _("   time       \n");
      _("\n");
      _("   children   This is the total amount of time spent in the descendants\n");
      _("   time       of this function.\n");
      _("\n");
      _("   calls      This is the number of times this function was invoked.\n");
      _("\n");
      _("   name       The name of the function.\n");
      _("\n");
      _(" For the functions callers, the fields have the following meaning:\n");
      _("   self       This is the amount of time that was propagated directly\n");
      _("   time       from the function into this caller.\n");
      _("\n");
      _("   children   This is the amount of time that was propagated from the\n");
      _("   time       functions children into this caller.\n");
      _("\n");
      _("   calls      This is the number of times this caller called the function,\n");
      _("              out of the total number this function was called.\n");
      _("\n");
      _("   name       The name of the caller.\n");
      _("\n");
      _(" If the caller of a function can not be determined, it will be registered\n");
      _(" as <spontaneous> in the name field.\n");
      _("\n");
      _(" For the functions callees, the fields have the following meaning:\n");
      _("   self       This is the amount of time that was propagated directly from\n");
      _("   time       this callee into the function.\n");
      _("\n");
      _("   children   This is the amount of time that was propagated from the\n");
      _("   time       callees children into the function.\n");
      _("\n");
      _("   calls      This is the total number of times the callee function was\n");
      _("              called from the function, of the total times the callee was\n");
      _("              called.\n");
      _("\n");
      _("   name       The name of the callee.\n");
      _("\n");
    }
}
