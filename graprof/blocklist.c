
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


#include "blocklist.h"

#include <string.h>

#include <grapes/util.h>

block *blocks = NULL;
unsigned int nblocks = 0;


block*
blocklist_get (unsigned int *n)
{
  *n = nblocks;
  return blocks;
}

block*
blocklist_get_by_address (uintptr_t address)
{
  unsigned int i;
  for (i = 0; i < nblocks; ++i)
    if (blocks[i].address == address)
      return blocks + i;

  return NULL;
}

block*
blocklist_add (uintptr_t address)
{
  ++nblocks;
  blocks = realloc(blocks, sizeof(*blocks) * nblocks);
  assert_inner_ptr(blocks, "realloc");

  block *b = blocks + nblocks - 1;

  b->address = address;

  return b;
}

void
blocklist_relocate (block *b, uintptr_t address)
{
  b->address = address;
}

void
blocklist_remove (block *b)
{
  if (b->direct_call)
  {
    free(b->file);
    free(b->func);
  }

  --nblocks;
  memmove(b, b + 1, (nblocks - (b - blocks)) * sizeof(*blocks));
}

static void
__attribute__((destructor))
blocklist_fini (void)
{
  unsigned int i;
  for (i = 0; i < nblocks; ++i)
    if (blocks[i].direct_call)
      {
        free(blocks[i].file);
        free(blocks[i].func);
      }
  free(blocks);
}
