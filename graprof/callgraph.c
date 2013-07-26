
#include "callgraph.h"

#include <stdio.h>

extern FILE *graprof_out;

int
callgraph_print ()
{
  fprintf(graprof_out, "Call graph:\n");

  return 0;
}
