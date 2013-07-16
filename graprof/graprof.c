
#include "graprof.h"

#include <grapes/feedback.h>

#include <stdio.h>

extern FILE *yyin;
extern int yylex(const char *filename);

int 
main (int argc, char *argv[])
{
  struct arguments args = { 0, 0, 0, 0 };
  argp_parse (&argp, argc, argv, 0, 0, &args);

  yyin = fopen(args.trace_filename, "r");
  feedback_assert(yyin, "%s", args.trace_filename);
  
  int res = yylex(args.trace_filename);
  feedback_assert(!res, "%s", args.trace_filename);

  FILE *out = stdout;
  if (args.out_filename && (args.tasks & ~GRAPROF_NO_GUI))
    {
      out = fopen(args.out_filename, "w");
      feedback_assert(out, "%s", args.out_filename);
    }

  if (args.tasks & GRAPROF_FLAT_PROFILE)
    {
      errno = ENOSYS;
      feedback_assert_wrn(0, "--flat-profile");
    }

  if (args.tasks & GRAPROF_CALL_TREE)
    {
      errno = ENOSYS;
      feedback_assert_wrn(0, "--call-tree");
    }

  if (args.tasks & GRAPROF_MEMORY_PROFILE)
    {
      errno = ENOSYS;
      feedback_assert_wrn(0, "--memory-profile");
    }

  if (out == stdout)
    fclose(out);

  if (!(args.tasks & GRAPROF_NO_GUI))
    {
      errno = ENOSYS;
      feedback_assert_wrn(0, "tracing gui");
    }

  return 0;
}
