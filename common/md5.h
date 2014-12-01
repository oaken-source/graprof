
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
#endif

/* digest the given file into an md5 hash and store it in the given buffer
 *
 * params:
 *   dest - a buffer of at least DIGEST_LENGTH bytes
 *   filename - the name of the file to digest
 *
 * errors:
 *   may fail and set errno for the same reasons as file_map and file_unmap
 *   fails and set errno to ENOSYS if no supported md5 implementation is found
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int md5_digest (unsigned char dest[DIGEST_LENGTH], const char *filename) __may_fail;
