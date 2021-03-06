
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


#pragma once

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdint.h>
#include <stdlib.h>

#include <grapes/util.h>

/* this struct holds all information related to an allocated block of memory
 */
struct block
{
  uintptr_t address;
  size_t size;

  unsigned int direct_call;
  uintptr_t caller;
  char *file;
  unsigned int line;
  char *func;
};

typedef struct block block;


/* get the list of allocated blocks
 *
 * params:
 *   nblocks - pointer where the size of the returned array is stored
 *
 * returns:
 *   a pointer to the first element in an array of known memory blocks
 */
block *blocklist_get(unsigned int *nblocks);

/* get a non-invalidated block identified by its unique memory address using
 * binary search
 *
 * params:
 *   address - the address of the block to query
 *
 * returns:
 *   a pointer to a block, if found and not invalidated, NULL otherwise
 */
block *blocklist_get_by_address(uintptr_t address);

/* add a block to the blocklist, reuse existing memory if appropriate
 *
 * params:
 *   address - the address of the new block
 *
 * errors:
 *   may fail and set errno for the same reasons as realloc
 *
 * returns:
 *   a pointer to the newly inserted, or recycled block structure on success,
 *   NULL on failure.
 */
block *blocklist_add(uintptr_t address) __may_fail;

/* update the address of a block of memory, e.g. after realloc, and reflect
 * this update in the sorted array of blocks
 *
 * params:
 *   b - a pointer to the moved block
 *   address - the new address
 *
 * errors:
 *   may fail and set errno for the same reasons as realloc
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int blocklist_relocate(block *b, uintptr_t address) __may_fail;

/* invalidate a block
 *
 * params:
 *   b - the block to invalidate
 */
void blocklist_remove(block *b);
