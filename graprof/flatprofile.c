
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


#include "flatprofile.h"

#include "function.h"
#include "trace.h"
#include "strtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

extern FILE *graprof_out;

#define _(...) fprintf(graprof_out, __VA_ARGS__)

void
flatprofile_print (int verbose)
{
  _("Flat profile:\n");
  _("\n");

  unsigned long long total_runtime = trace_get_total_runtime();
  const char *total_runtime_prefix = strtime(&total_runtime);

  unsigned int nfunctions = function_get_nfunctions();

  _(" total runtime:                      %llu %sseconds\n", total_runtime, total_runtime_prefix);
  _(" total number of function calls:     %llu\n", function_get_total_calls());
  _(" total number of distinct functions: %u\n", nfunctions);
  _("\n");
  _("  %%       self    children             self    children\n");
  _(" time      time      time     calls    /call     /call  name\n");

  function **sorted_functions = function_get_all_sorted();
  function *functions = function_get_all();

  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
    {
      function *f = sorted_functions[i];

      unsigned long long total_runtime = trace_get_total_runtime();
      float percent_time = (100.0 * f->self_time) / total_runtime;

      unsigned long long self_time = f->self_time;
      const char *self_time_prefix = strtime(&self_time);

      unsigned long long children_time = f->children_time;
      const char *children_time_prefix = strtime(&children_time);

      unsigned long long calls = f->calls;

      unsigned long long self_per_call = f->self_time / f->calls;
      const char *self_per_call_prefix = strtime(&self_per_call);

      unsigned long long children_per_call = f->children_time / f->calls;
      const char *children_per_call_prefix = strtime(&children_per_call);

      unsigned int index = f - functions;

      _("%6.2f %6llu %ss %6llu %ss %8llu %6llu %ss %6llu %ss  ",
          percent_time, self_time, self_time_prefix, children_time, children_time_prefix,
          calls, self_per_call, self_per_call_prefix, children_per_call, children_per_call_prefix);

      if (!strcmp(f->name, "??"))
        _("0x%" PRIxPTR, f->address);
      else
        _("%s", f->name);

      _(" [%u]\n", index);
    }

  if (verbose >= 1)
    {
      _("\n");
      _(" %%          the percentage of the total running time of the\n");
      _(" time       program spent in this function\n");
      _("\n");
      _(" self       the absolute total running time spent in this\n");
      _(" time       function alone - this is the major sort of this\n");
      _("            listing\n");
      _("\n");
      _(" children   the absolute total running time of the program\n");
      _(" time       spent in the descendants of this function in the\n");
      _("            call tree\n");
      _("\n");
      _(" calls      the number of times this function was invoked\n");
      _("\n");
      _(" self       the average running time spent in this function\n");
      _(" /call      per call\n");
      _("\n");
      _(" chlidren   the average running time spent in the descendants\n");
      _(" /call      of this function in the call tree per call\n");
      _("\n");
      _(" name       the name of the function, if available, else its\n");
      _("            address - this is the minor sort of this listing\n");
    }

  _("\n");
}

#undef  _
