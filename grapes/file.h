
/******************************************************************************
 *                                  Grapes                                    *
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

/* This is a convenience wrapper for the frequently used but cumbersome mmap
 * function.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <grapes/util.h>

#include <stdlib.h>

/* maps a file read-only to a buffer via mmap, use file_unmap to free; the
 * behaviour of this function diverges from the original mmap functionality
 * in that it succeeds to map empty files (with length zero) and returns a
 * pointer to an immutable const char* pointing to an empty string. It is safe
 * to pass this returned pointer to file_unmap.
 *
 * params:
 *   filename - the path to the file to map
 *   length - a pointer where the size of the allocation is stored in
 *
 * errors:
 *   may fail and set errno for the same reasons as open, fstat and mmap
 *   the behaviour is undefined if the length poiner is invalid
 *
 * returns:
 *   a pointer to the mapped area, if successful, NULL otherwise
 */
void *file_map(const char *filename, size_t *length) __may_fail;

/* unmaps a memory area previously mapped with file_map
 *
 * params:
 *   data - the pointer reurned by file_map
 *   length - the length returned by file_map
 *
 * errors:
 *   may fail and set errno for the same reasons as munmap
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int file_unmap(void *data, size_t length) __may_fail;
