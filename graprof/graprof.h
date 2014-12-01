
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


#pragma once

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <grapes/util.h>
#include <grapes/feedback.h>

#include <argp.h>
#include <stdlib.h>

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;

const char doc[] = PACKAGE_NAME " - a profiling and trace analysis tool";
const char args_doc[] = "[--] <binary call>";

static struct argp_option options[] =
{
  {"flat-profile", 'F', 0, 0, "generate flat profile from trace data", 0},
  {"call-graph", 'C', 0, 0, "generate call graph from trace data", 0},
  {"memory-profile", 'M', 0, 0, "generate flat memory profile from trace data", 0},
  {"tracing-gui", 'G', 0, 0, "open the trace explorer gui", 0},
  {"output", 'o', "<file>", 0, "write profile results to <file> instead of stdout", 0},
  {"verbose", 'v', 0, 0, "be more verbose", 0},
  {"verbose", 'q', 0, 0, "be less verbose", 0},
  {"trace", 't', "<file>", 0, "use a given tracefile instead of running the given binary to generate a trace", 0},
  {0, 0, 0, 0, 0, 0}
};

enum graprof_tasks
{
  GRAPROF_FLAT_PROFILE      = 0x01,
  GRAPROF_CALL_GRAPH        = 0x02,
  GRAPROF_MEMORY_PROFILE    = 0x04,
  GRAPROF_TRACING_GUI       = 0x08
};
typedef enum graprof_tasks graprof_tasks;

struct arguments
{
  const char **binary_invocation;
  const char *trace_filename;

  graprof_tasks tasks;
  const char *out_filename;

  int verbose;
};

#define check_arg_is_not_after_binary(S) \
    do { \
      if ((S)->arg_num) \
        { \
          feedback_error(EXIT_SUCCESS, "parameter `%s' after binary", (S)->argv[(S)->next - 1]); \
          argp_usage(state); \
        } \
    } while (0)

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *args = (struct arguments*)state->input;

  switch(key)
    {
    case 'F':
      check_arg_is_not_after_binary(state);
      args->tasks |= GRAPROF_FLAT_PROFILE;
      break;
    case 'C':
      check_arg_is_not_after_binary(state);
      args->tasks |= GRAPROF_CALL_GRAPH;
      break;
    case 'M':
      check_arg_is_not_after_binary(state);
      args->tasks |= GRAPROF_MEMORY_PROFILE;
      break;
    case 'G':
      check_arg_is_not_after_binary(state);
      args->tasks |= GRAPROF_TRACING_GUI;
      break;
    case 'o':
      check_arg_is_not_after_binary(state);
      args->out_filename = arg;
      break;
    case 'v':
      check_arg_is_not_after_binary(state);
      ++(args->verbose);
      break;
    case 'q':
      check_arg_is_not_after_binary(state);
      --(args->verbose);
    case 't':
      check_arg_is_not_after_binary(state);
      args->trace_filename = arg;
      break;
    case ARGP_KEY_ARG:
      args->binary_invocation = realloc(args->binary_invocation, sizeof(const char*) * (state->arg_num + 2));
      feedback_assert(args->binary_invocation, "argp");
      args->binary_invocation[state->arg_num] = arg;
      args->binary_invocation[state->arg_num + 1] = NULL;
      break;
    case ARGP_KEY_END:
      if (!state->arg_num)
        argp_usage(state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

#undef  check_arg_is_not_after_binary

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};
