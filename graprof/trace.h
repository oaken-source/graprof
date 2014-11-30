
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

#include <grapes/util.h>

#include <common/tracebuffer.h>

/* read trace data from a given file and distribute the contained information
 * to the appropriate submodules.
 *
 * params:
 *   filename - the file that contains the trace data
 *
 * errors:
 *   ENOTSUP - the trace data is somehow invalid
 *   may also fail and set errno for the same reasons as fopen and malloc
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int trace_read(const char *filename, unsigned char md5_binary[DIGEST_LENGTH]) __may_fail;

/* get the total number of ns elapsed from start to end of the program
 *
 * returns:
 *   the time value associated with the end trace symbol
 */
unsigned long long trace_get_total_runtime(void);
