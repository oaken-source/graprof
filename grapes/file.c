
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


#include "file.h"

#include "util.h" // <grapes/util.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

const char *file_map_empty = "";

void*
file_map (const char *filename, size_t *length)
{
  __returns_ptr;

  __precondition(EINVAL, NULL != filename);
  __precondition(EINVAL, NULL != length);

  int fd;
  struct stat sb;
  void *data;

  *length = 0;

  __checked_call(-1 != (fd = open(filename, O_RDONLY)));
  __checked_call(-1 != fstat(fd, &sb),
    close(fd);
  );

  if (sb.st_size == 0)
    return (void*)file_map_empty;

  __checked_call(MAP_FAILED != (data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0)),
    close(fd);
  );

  *length = sb.st_size;
  return data;
}

int
file_unmap (void *data, size_t length)
{
  if (data && (data != file_map_empty))
    __checked_call(0 == munmap(data, length));

  return 0;
}

