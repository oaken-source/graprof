
/******************************************************************************
 *                                  Grapes                                    *
 *                                                                            *
 *    Copyright (C) 2013 Andreas Grapentin                                    *
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


#include "file.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

void*
file_map (const char *filename, size_t *length)
{
  int fd = open(filename, O_RDONLY);
  assert_inner_ptr(fd != -1, "%s: open", filename);

  struct stat sb;
  int res = fstat(fd, &sb);
  assert_inner_ptr(res != -1, "%s: fstat", filename);

  *length = sb.st_size;

  void *data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert_inner_ptr(data != MAP_FAILED, "%s: mmap", filename);

  return data;
}

void
file_unmap (void *data, size_t length)
{
  munmap(data, length);
}

