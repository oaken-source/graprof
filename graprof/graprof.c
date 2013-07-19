
#include "graprof.h"

#include "timeline.h"
#include "addr.h"

#include <grapes/feedback.h>

#include <stdio.h>

extern FILE *yyin;
extern int yylex(const char *filename);

FILE *graprof_out;

int 
main (int argc, char *argv[])
{
  struct arguments args = { 0, 0, 0, 0 };
  argp_parse (&argp, argc, argv, 0, 0, &args);

  yyin = fopen(args.trace_filename, "r");
  feedback_assert(yyin, "%s", args.trace_filename);
  
  int res = yylex(args.trace_filename);
  feedback_assert(!res, "%s", args.trace_filename);
 
  if (args.binary_filename)
    {
      int res = addr_init(args.binary_filename);
      if (res)
        {
          if (errno == ENOTSUP)
            {
              errno = 0;
              feedback_warning("%s: file format not supported or no debug symbols found", args.binary_filename);
            }
          else
            {
              feedback_warning("%s", args.binary_filename);
            }
        }
    }

  timeline_event *e = timeline_head();

  char *function;
  char *file;
  unsigned int line;
  res = addr_translate(e->func, &function, &file, &line);

  printf("0x%" PRIxPTR " : %s\n%s:%u\n", e->func, function, file, line);

  graprof_out = stdout;
  if (args.out_filename && (args.tasks & ~GRAPROF_NO_GUI))
    {
      graprof_out = fopen(args.out_filename, "w");
      feedback_assert(graprof_out, "%s", args.out_filename);
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

  if (graprof_out != stdout)
    fclose(graprof_out);

  if (!(args.tasks & GRAPROF_NO_GUI))
    {
      errno = ENOSYS;
      feedback_assert_wrn(0, "tracing gui");
    }

  return 0;
}
