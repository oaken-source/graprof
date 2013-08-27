
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

#include <config.h>

extern void *libgraprof_buf;
extern unsigned long libgraprof_bufsize;

/* these macros are used by the intrumentation and the malloc hooks to
 * build an in-memory trace data block, that is written to disk upon
 * program termination.
 */

#define buffer_enlarge(S) \
  unsigned long index = libgraprof_bufsize; \
  libgraprof_bufsize += (S); \
  libgraprof_buf = realloc(libgraprof_buf, libgraprof_bufsize)

#define buffer_append(T, V) \
  *((T*)(libgraprof_buf + index)) = (V); \
  index += sizeof(T)
