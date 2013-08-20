
#include "memprofile.h"

#include "memory.h"

#include <stdio.h>
#include <inttypes.h>

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

  unsigned int nfailed_mallocs = 0;
  failed_malloc *failed_mallocs = memory_get_failed_mallocs(&nfailed_mallocs);

  unsigned int i;
  for (i = 0; i < nfailed_mallocs; ++i)
    fprintf(graprof_out, " %s:%u: %sfailed to allocate a block of %zu bytes\n", failed_mallocs[i].file, failed_mallocs[i].line, (failed_mallocs[i].direct_call ? "" : "library call "), failed_mallocs[i].size);

  if (nfailed_mallocs)
    fprintf(graprof_out, "\n");

  unsigned int nfailed_reallocs = 0;
  failed_realloc *failed_reallocs = memory_get_failed_reallocs(&nfailed_reallocs);

  for (i = 0; i < nfailed_reallocs; ++i)
    if (failed_reallocs[i].reason == FAILED_INVALID_PTR)
      fprintf(graprof_out, " %s:%u: %sfailed to reallocate a block at 0x%" PRIxPTR ", which was never allocated\n", failed_reallocs[i].file, failed_reallocs[i].line, (failed_reallocs[i].direct_call ? "" : "library call "), failed_reallocs[i].ptr);
    else
      fprintf(graprof_out, " %s:%u: %sfailed to reallocate a block at 0x%" PRIxPTR " of size %zu to size %zu\n", failed_reallocs[i].file, failed_reallocs[i].line, (failed_reallocs[i].direct_call ? "" : "library call "), failed_reallocs[i].ptr, failed_reallocs[i].start_size, failed_reallocs[i].end_size);
      

  if (nfailed_reallocs)
    fprintf(graprof_out, "\n");

  fprintf(graprof_out, "\n");
}

