
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
extern int graprof_verbosity;

extern struct arguments arguments;

static const char *flatprofile_str = NULL;

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

static size_t
flatprofile_snprintf (char *buffer, size_t length)
{
  size_t pos = 0;
  pos += snprintf(buffer + pos, length,
      "Flat profile:\n"
      "\n");

  const char *prefix;
  unsigned long long time = trace_get_total_runtime();
  strtime(&time, &prefix);

  pos += snprintf(buffer + pos, length,
      " total runtime:                      %llu %sseconds\n"
      " total number of function calls:     %llu\n"
      " total number of distinct functions: %u\n"
      "\n"
      "  %%       self    children             self    children\n"
      " time      time      time     calls    /call     /call  name\n",
      time, prefix,
      function_get_total_calls(),
      function_get_nfunctions());

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
      pos += snprintf(buffer + pos, length, "%6.2f ", (100.0 * f->self_time) / time);

      time = f->self_time;
      strtime(&time, &prefix);

      pos += snprintf(buffer + pos, length, "%6llu %ss ", time, prefix);

      time = f->children_time;
      strtime(&time, &prefix);

      pos += snprintf(buffer + pos, length, "%6llu %ss ", time, prefix);

      pos += snprintf(buffer + pos, length, "%8lu ", f->calls);

      time = f->self_time / f->calls;
      strtime(&time, &prefix);

      pos += snprintf(buffer + pos, length, "%6llu %ss ", time, prefix);

      time = f->children_time / f->calls;
      strtime(&time, &prefix);

      pos += snprintf(buffer + pos, length, "%6llu %ss  ", time, prefix);

      if (!strcmp(f->name, "??"))
        pos += snprintf(buffer + pos, length, "0x%" PRIxPTR , f->address);
      else
        pos += snprintf(buffer + pos, length, "%s", f->name);

      pos += snprintf(buffer + pos, length, " [%u]", (unsigned int)(f - functions));

      pos += snprintf(buffer + pos, length, "\n");
    }

  free(sorted_functions);

  if (graprof_verbosity >= 1)
    {
      pos += snprintf(buffer + pos, length,
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
        "            address - this is the minor sort of this listing\n"
        "\n");
    }

  return pos + 1;
}

static int may_fail
flatprofile_generate (void)
{
  size_t flatprofile_length = flatprofile_snprintf(NULL, 0);

  flatprofile_str = calloc(flatprofile_length, sizeof(*flatprofile_str));
  assert_inner(flatprofile_str, "calloc");

  flatprofile_snprintf((char*)flatprofile_str, flatprofile_length);

  return 0;
}

const char*
flatprofile_as_str (void)
{
  if (!flatprofile_str)
    {
      int res = flatprofile_generate();
      assert_inner_ptr(!res, "flatprofile_generate");
    }

  return flatprofile_str;
}

int
flatprofile_print (void)
{
  if (!flatprofile_str)
    {
      int res = flatprofile_generate();
      assert_inner(!res, "flatprofile_generate");
    }

  fprintf(graprof_out, "%s\n", flatprofile_str);

  return 0;
}

