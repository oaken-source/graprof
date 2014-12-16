
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


#include "digest.h"

#include <grapes/file.h>

#include <string.h>

int
digest_file (digest_t *dest, const char *filename)
{
  size_t length;
  void *data;

  __checked_call(NULL != (data = file_map(filename, &length)));

  #if HAVE_OPENSSL_MD5

    MD5(data, length, *dest);

  #elif HAVE_BSD_MD5

    MD5Data(data, length, (void*)*dest);

  #endif

  __checked_call(0 == file_unmap(data, length));

  return 0;
}

int
digest_cmp (const digest_t digest1, const digest_t digest2)
{
  return memcmp(digest1, digest2, DIGEST_LENGTH);
}
