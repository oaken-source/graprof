
#include "memprofile.h"

#include "memory.h"

#include <stdio.h>

extern FILE *graprof_out;

void
memprofile_print ()
{
  fprintf(graprof_out, "Memory profile:\n");
  fprintf(graprof_out, "\n"); 

  fprintf(graprof_out, " total bytes allocated:   %llu\n", memory_get_total_allocated());
  fprintf(graprof_out, " total bytes freed:       %llu\n", memory_get_total_freed());
  fprintf(graprof_out, " maximum bytes allocated: %llu\n", memory_get_maximum_allocated());
  fprintf(graprof_out, "\n");
  fprintf(graprof_out, " calls to malloc:         %u\n", memory_get_total_allocations());
  fprintf(graprof_out, " calls to realloc:        %u\n", memory_get_total_reallocations());
  fprintf(graprof_out, " calls to free:           %u\n", memory_get_total_frees());

  fprintf(graprof_out, "\n");
}

