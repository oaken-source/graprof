
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

int
flatprofile_print (int verbose)
{
  fprintf(graprof_out,
      "Flat profile:\n"
      "\n");

  unsigned long long total_runtime = trace_get_total_runtime();
  const char *total_runtime_prefix = strtime(&total_runtime);

  unsigned int nfunctions = function_get_nfunctions();

  fprintf(graprof_out,
      " total runtime:                      %llu %sseconds\n"
      " total number of function calls:     %llu\n"
      " total number of distinct functions: %u\n"
      "\n"
      "  %%       self    children             self    children\n"
      " time      time      time     calls    /call     /call  name\n",
      total_runtime, total_runtime_prefix,
      function_get_total_calls(),
      nfunctions);

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

      fprintf(graprof_out,
        "%6.2f %6llu %ss %6llu %ss %8llu %6llu %ss %6llu %ss  ",
        percent_time, self_time, self_time_prefix, children_time, children_time_prefix,
        calls, self_per_call, self_per_call_prefix, children_per_call, children_per_call_prefix);

      if (!strcmp(f->name, "??"))
        fprintf(graprof_out, "0x%" PRIxPTR, f->address);
      else
        fprintf(graprof_out, "%s", f->name);

      fprintf(graprof_out, " [%u]\n", index);
    }

  if (verbose >= 1)
    {
      fprintf(graprof_out,
        "\n"
        " %%          the percentage of the total running time of the\n"
        " time       program spent in this function\n"
        "\n"
        " self       the absolute total running time spent in this\n"
        " time       function alone - this is the major sort of this\n"
        "            listing\n"
        "\n"
        " children   the absolute total running time of the program\n"
        " time       spent in the descendants of this function in the\n"
        "            call tree\n"
        "\n"
        " calls      the number of times this function was invoked\n"
        "\n"
        " self       the average running time spent in this function\n"
        " /call      per call\n"
        "\n"
        " chlidren   the average running time spent in the descendants\n"
        " /call      of this function in the call tree per call\n"
        "\n"
        " name       the name of the function, if available, else its\n"
        "            address - this is the minor sort of this listing\n");
    }

  fprintf(graprof_out, "\n");

  return 0;
}

