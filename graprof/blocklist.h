
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


#pragma once

#include <stdint.h>
#include <stdlib.h>

/* this struct holds all information related to an allocated block of memory
 */
struct block
{
  uintptr_t address;
  size_t size;

  unsigned int direct_call;
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

block *blocklist_get_by_address(uintptr_t address);

block *blocklist_add(uintptr_t address);

void blocklist_relocate(block *b, uintptr_t address);

void blocklist_remove(block *b);
