
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


#include "md5.h"

#include <grapes/feedback.h>
#include <grapes/file.h>

void
md5_digest (unsigned char *dest, const char *filename)
{
  size_t length;
  void *data;

  data = file_map(filename, &length);
  if (!data)
    {
      feedback_warning("libgraprof: %s", filename);
      return;
    }

  #if HAVE_OPENSSL_MD5
    MD5(data, length, dest);
  #elif HAVE_BSD_MD5
    MD5Data(data, length, (void*)dest);
  #endif

  int res = file_unmap(data, length);
  feedback_assert_wrn(!res, "libgraprof: %s", filename);
}
