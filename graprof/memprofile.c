
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013  Andreas Grapentin                                   *
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
    {
      fprintf(graprof_out, " %s:%u: ", failed_reallocs[i].file, failed_reallocs[i].line);
      if (!failed_reallocs[i].direct_call)
        fprintf(graprof_out, "library call ");
      fprintf(graprof_out, "failed to reallocate a block at 0x%" PRIxPTR, failed_reallocs[i].ptr);
      if (failed_reallocs[i].reason == FAILED_INVALID_PTR)
        fprintf(graprof_out, ", which was never allocated\n");
      else
        fprintf(graprof_out, " of size %zu to size %zu\n", failed_reallocs[i].start_size, failed_reallocs[i].end_size);
    }
      
  if (nfailed_reallocs)
    fprintf(graprof_out, "\n");

  unsigned int nfailed_frees = 0;
  failed_free *failed_frees = memory_get_failed_frees(&nfailed_frees);

  for (i = 0; i < nfailed_frees; ++i)
    {
      fprintf(graprof_out, " %s:%u: ", failed_frees[i].file, failed_frees[i].line);
      if (!failed_frees[i].direct_call)
        fprintf(graprof_out, "library_call ");
      fprintf(graprof_out, "failed to free a block at 0x%" PRIxPTR, failed_frees[i].ptr);
      if (failed_frees[i].reason == FAILED_INVALID_PTR)
        fprintf(graprof_out, ", which was never allocated\n");
      else
        fprintf(graprof_out, ", which was already freed\n");
    }

  fprintf(graprof_out, "\n");
}
