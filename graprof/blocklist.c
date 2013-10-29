
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
  int left = 0;
  int right = nblocks - 1;
  int i = 0;

  while (left <= right)
    {
      i = (left + right) / 2;
      if (blocks[i].address == address)
        return (blocks[i].size ? blocks + i : NULL);
      else if (blocks[i].address > address)
        right = i - 1;
      else
        left = i + 1;
    }

  return NULL;
}

block*
blocklist_add (uintptr_t address)
{
  int left = 0;
  int right = nblocks - 1;
  int i = 0;

  while (left <= right)
    {
      i = (left + right) / 2;
      if (blocks[i].address == address)
        return blocks + i;
      else if (blocks[i].address > address)
        right = i - 1;
      else
        left = i + 1;
    }

  ++nblocks;
  blocks = realloc(blocks, sizeof(*blocks) * nblocks);
  assert_inner_ptr(blocks, "realloc");

  i = left;

  memmove(blocks + i + 1, blocks + i, sizeof(*blocks) * (nblocks - i - 1));

  block *b = blocks + i;
  memset(b, 0, sizeof(*b));

  b->address = address;

  return b;
}

int
blocklist_relocate (block *b, uintptr_t address)
{
  block tmp = *b;
  tmp.address = address;

  b->size = 0;
  b->direct_call = 0;

  block *new = blocklist_add(address);
  assert_inner(new, "blocklist_add");

  *new = tmp;

  return 0;
}

void
blocklist_remove (block *b)
{
  if (b->direct_call)
  {
    free(b->file);
    free(b->func);
  }

  b->size = 0;
  b->direct_call = 0;
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
