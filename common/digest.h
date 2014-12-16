
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

#if HAVE_OPENSSL_MD5
#  include <openssl/md5.h>
#elif HAVE_BSD_MD5
#  include <bsd/md5.h>
#else
#  error required md5 implementation not found
#endif

/* the type of a digest
 */
typedef unsigned char digest_t[DIGEST_LENGTH];

/* digest the given file and store the result it in the given digest_t
 *
 * params:
 *   dest - a pointer to a digest_t
 *   filename - the name of the file to digest
 *
 * errors:
 *   may fail and set errno for the same reasons as file_map and file_unmap
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int digest_file (digest_t *dest, const char *filename) __may_fail;

/* compare two digests
 *
 * params:
 *   d1 - the first digest
 *   d2 - the second digest
 *
 * returns:
 *   an integer less than, equal to, or greater than zero if d1 is found,
 *   respectively, to be less than, to match, or be greater than d2
 */
int digest_cmp (const digest_t d1, const digest_t d2);
