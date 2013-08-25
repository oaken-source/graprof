
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

/* initialize the bfd library and read the sybol table from the file specified
 * by <filename>
 *
 * params:
 *   filename - the binary file to read debug symbols from
 *
 * errors:
 *   This is black magic. It may fail, but as of yet I do not know how or why,
 *   except for some obvious cases like wrong filename or missing debug
 *   symbols. If it fails, and you know why, but the program can not tell you,
 *   please file a bug.
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int addr_init(const char *filename);

/* translate an address specified by <pc> to a function name, file name and 
 * line via the debug sybols loaded by addr_init. Memory for function and 
 * file is allocated by this function - remember to free it afterwards.
 *
 * params:
 *   pc - the address to look up in the debug symbols
 *   function - a pointer where the name of the function will be stored. If
 *     it can not be determined, it will be "??"
 *   file - analogous to function
 *   line - analogous to function, but the fallback value is 0
 *
 * errors:
 *   Again, there is black magic involved. If something feels weird to you,
 *   file a bug.
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int addr_translate(uintptr_t pc, char **function, char **file, unsigned int *line);
