
#pragma once

#include <config.h>

#include <argp.h>

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_adress = PACKAGE_BUGREPORT;

const char doc[] = PACKAGE_NAME " - a free system trace analysis and visualization tool";

const char args_doc[] = "[binary] trace";

static struct argp_option options[] = 
{
    {"flat-profile", 'F', 0, 0, "generate flat profile from trace data", 0},
    {"call-tree", 'C', 0, 0, "generete call tree from trace data", 0},
    {"memory-profile", 'M', 0, 0, "generate flat memory profile from trace data", 0},
    {"no-gui", 'g', 0, 0, "do not open the trace explorer gui", 0},
    {"output", 'o', "<file>", 0, "write profile results to <file> instead of stdout", 0},
    {0, 0, 0, 0, 0, 0}
};

#define GRAPROF_FLAT_PROFILE      0x01
#define GRAPROF_CALL_TREE         0x02
#define GRAPROF_MEMORY_PROFILE    0x04
#define GRAPROF_NO_GUI            0x08

struct arguments
{
  const char *binary_filename;
  const char *trace_filename;

  int tasks;
  const char *out_filename;
};

static error_t 
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = (struct arguments*)state->input;

  switch(key)
    {
    case 'F':
      arguments->tasks |= GRAPROF_FLAT_PROFILE;
      break;
    case 'C':
      arguments->tasks |= GRAPROF_CALL_TREE;
      break;
    case 'M':
      arguments->tasks |= GRAPROF_MEMORY_PROFILE;
      break;
    case 'g':
      arguments->tasks |= GRAPROF_NO_GUI;
      break;
    case 'o':
      arguments->out_filename = arg;
      break;
    case ARGP_KEY_ARG:
      switch (state->arg_num)
        {
        case 0:
          arguments->trace_filename = arg;
          break;
        case 1:
          arguments->binary_filename = arguments->trace_filename;
          arguments->trace_filename = arg;
          break;
        default:
          argp_usage(state);
        }
      break;
    case ARGP_KEY_END:
      if (state->arg_num < 1)
        argp_usage(state);
    default:
      return ARGP_ERR_UNKNOWN;
    }

  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

