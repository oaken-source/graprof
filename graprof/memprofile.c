
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013 - 2014  Andreas Grapentin                            *
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
#include "blocklist.h"

#include <stdio.h>
#include <inttypes.h>

extern FILE *graprof_out;

#define _(...) fprintf(graprof_out, __VA_ARGS__)

void
memprofile_print (void)
{
  _("Memory profile:");
  _("\n");
  _(" total bytes allocated:   %llu\n", memory_get_total_allocated());
  _(" total bytes freed:       %llu\n", memory_get_total_freed());
  _(" maximum bytes allocated: %llu\n", memory_get_maximum_allocated());
  _("\n");
  _(" calls to malloc:         %u\n", memory_get_total_allocations());
  _(" calls to realloc:        %u\n", memory_get_total_reallocations());
  _(" calls to free:           %u\n", memory_get_total_frees());
  _("\n");

  unsigned int nfailed_mallocs;
  failed_malloc *failed_mallocs = memory_get_failed_mallocs(&nfailed_mallocs);

  unsigned int i;
  for (i = 0; i < nfailed_mallocs; ++i)
    {
      if (failed_mallocs[i].direct_call)
        {
          _(" %s:%u:%s: ", failed_mallocs[i].file, failed_mallocs[i].line, failed_mallocs[i].func);
        }
      else
        {
          _(" 0x%" PRIxPTR ": ", failed_mallocs[i].caller);
          if (failed_mallocs[i].func)
            _("last parent %s:%u:%s: ", failed_mallocs[i].file, failed_mallocs[i].line, failed_mallocs[i].func);
          else
            _("last parent unknown: ");
        }

      _("failed to allocate a block of %zu bytes\n", failed_mallocs[i].size);
    }

  if (nfailed_mallocs)
    _("\n");

  unsigned int nfailed_reallocs = 0;
  failed_realloc *failed_reallocs = memory_get_failed_reallocs(&nfailed_reallocs);

  for (i = 0; i < nfailed_reallocs; ++i)
    {
      if (failed_reallocs[i].direct_call)
        {
          _(" %s:%u:%s: ", failed_reallocs[i].file, failed_reallocs[i].line, failed_reallocs[i].func);
        }
      else
        {
          _(" 0x%" PRIxPTR ": ", failed_reallocs[i].caller);
          if (failed_reallocs[i].func)
            _("last parent %s:%u:%s: ", failed_reallocs[i].file, failed_reallocs[i].line, failed_reallocs[i].func);
          else
            _("last parent unknown: ");
        }

      _("failed to reallocate a block at 0x%" PRIxPTR, failed_reallocs[i].ptr);
      if (failed_reallocs[i].reason == FAILED_INVALID_PTR)
        _(", already free'd or invalid ptr\n");
      else
        _(" of size %zu to size %zu\n", failed_reallocs[i].start_size, failed_reallocs[i].end_size);
    }

  if (nfailed_reallocs)
    _("\n");

  unsigned int nfailed_frees = 0;
  failed_free *failed_frees = memory_get_failed_frees(&nfailed_frees);

  for (i = 0; i < nfailed_frees; ++i)
    {
      if (failed_frees[i].direct_call)
        {
          _(" %s:%u:%s: ", failed_frees[i].file, failed_frees[i].line, failed_frees[i].func);
        }
      else
        {
          _(" 0x%" PRIxPTR ": ", failed_frees[i].caller);
          if (failed_frees[i].func)
            _("last parent %s:%u:%s: ", failed_frees[i].file, failed_frees[i].line, failed_frees[i].func);
          else
            _("last parent unknown: ");
        }

      _("failed to free a block at 0x%" PRIxPTR ", double free or invalid ptr\n", failed_frees[i].ptr);
    }

  if (nfailed_frees)
    _("\n");

  unsigned int nblocks = 0;
  block *blocks = blocklist_get(&nblocks);

  unsigned int nblocks_left = 0;
  for (i = 0; i < nblocks; ++i)
    {
      if (blocks[i].size)
        {
          ++nblocks_left;
          if (blocks[i].direct_call)
            {
              _(" %s:%u:%s: ", blocks[i].file, blocks[i].line, blocks[i].func);
            }
          else
            {
              _(" 0x%" PRIxPTR ": ", blocks[i].caller);
              if (blocks[i].func)
                _("last parent %s:%u:%s: ", blocks[i].file, blocks[i].line, blocks[i].func);
              else
                _("last parent unknown: ");
            }

          _("allocated block at 0x%" PRIxPTR " of size %zu that was never freed\n", blocks[i].address, blocks[i].size);
        }
    }

  if (nblocks_left)
    _("\n");
}

#undef  _
