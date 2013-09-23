
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
memprofile_print (void)
{
  fprintf(graprof_out,
        "Memory profile:\n"
        "\n"
        " total bytes allocated:   %llu\n"
        " total bytes freed:       %llu\n"
        " maximum bytes allocated: %llu\n"
        "\n"
        " calls to malloc:         %u\n"
        " calls to realloc:        %u\n"
        " calls to free:           %u\n"
        "\n",
        memory_get_total_allocated(),
        memory_get_total_freed(),
        memory_get_maximum_allocated(),
        memory_get_total_allocations(),
        memory_get_total_reallocations(),
        memory_get_total_frees());

  unsigned int nfailed_mallocs = 0;
  failed_malloc *failed_mallocs = memory_get_failed_mallocs(&nfailed_mallocs);

  unsigned int i;
  for (i = 0; i < nfailed_mallocs; ++i)
    fprintf(graprof_out, " %s:%u:%s: %sfailed to allocate a block of %zu bytes\n", failed_mallocs[i].file, failed_mallocs[i].line, failed_mallocs[i].func, (failed_mallocs[i].direct_call ? "" : "library call "), failed_mallocs[i].size);

  if (nfailed_mallocs)
    fprintf(graprof_out, "\n");

  unsigned int nfailed_reallocs = 0;
  failed_realloc *failed_reallocs = memory_get_failed_reallocs(&nfailed_reallocs);

  for (i = 0; i < nfailed_reallocs; ++i)
    {
      fprintf(graprof_out, " %s:%u:%s: ", failed_reallocs[i].file, failed_reallocs[i].line, failed_reallocs[i].func);
      if (!failed_reallocs[i].direct_call)
        fprintf(graprof_out, "library call ");
      fprintf(graprof_out, "failed to reallocate a block at 0x%" PRIxPTR, failed_reallocs[i].ptr);
      if (failed_reallocs[i].reason == FAILED_INVALID_PTR)
        fprintf(graprof_out, ", already free'd or invalid ptr\n");
      else
        fprintf(graprof_out, " of size %zu to size %zu\n", failed_reallocs[i].start_size, failed_reallocs[i].end_size);
    }

  if (nfailed_reallocs)
    fprintf(graprof_out, "\n");

  unsigned int nfailed_frees = 0;
  failed_free *failed_frees = memory_get_failed_frees(&nfailed_frees);

  for (i = 0; i < nfailed_frees; ++i)
    {
      fprintf(graprof_out, " %s:%u:%s: ", failed_frees[i].file, failed_frees[i].line, failed_frees[i].func);
      if (!failed_frees[i].direct_call)
        fprintf(graprof_out, "library call ");
      fprintf(graprof_out, "failed to free a block at 0x%" PRIxPTR, failed_frees[i].ptr);
      fprintf(graprof_out, ", double free or invalid ptr\n");
    }

  if (nfailed_frees)
    fprintf(graprof_out, "\n");

  unsigned int nblocks = 0;
  block *blocks = memory_get_blocks(&nblocks);

  for (i = 0; i < nblocks; ++i)
    {
      fprintf(graprof_out, " %s:%u:%s: ", blocks[i].file, blocks[i].line, blocks[i].func);
      if (!blocks[i].direct_call)
        fprintf(graprof_out, "library call ");
      fprintf(graprof_out, "allocated block at 0x%" PRIxPTR " of size %zu that was never freed\n", blocks[i].address, blocks[i].size);
    }

  if (nblocks)
    fprintf(graprof_out, "\n");
}
