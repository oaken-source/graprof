
#pragma once

#include <config.h>

#include <argp.h>

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_adress = PACKAGE_BUGREPORT;

const char doc[] = PACKAGE_NAME " - a free system trace analysis and visualization tool";

const char args_doc[] = "[binary] trace";

static struct argp_option options[] = 
{
    {0, 0, 0, 0, 0, 0}
};

struct arguments
{
  const char *binary_filename;
  const char *trace_filename;
};

static error_t 
parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = (struct arguments*)state->input;

  switch(key)
    {
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

