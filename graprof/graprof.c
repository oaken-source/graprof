
#include "graprof.h"

#include <grapes/feedback.h>

#include <stdio.h>

extern FILE *yyin;
extern int yylex(const char *filename);

int 
main (int argc, char *argv[])
{
  struct arguments args = { 0, 0 };
  argp_parse (&argp, argc, argv, 0, 0, &args);

  yyin = fopen(args.trace_filename, "r");
  feedback_assert(yyin, "%s", args.trace_filename);
  
  int res = yylex(args.trace_filename);
  feedback_assert(!res, "%s", args.trace_filename);

  return 0;
}
