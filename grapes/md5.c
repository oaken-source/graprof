
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


#include "md5.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

/* $Id: md5.c,v 1.3 2006-05-01 16:57:31 quentin Exp $ */

/*
  * Implementation of the md5 algorithm as described in RFC1321
  * Copyright (C) 2005 Quentin Carbonneaux <crazyjoke@free.fr>
  * 
 * This file is part of md5sum.
  *
  * md5sum is a free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Softawre Foundation; either version 2 of the License, or
  * (at your option) any later version.
  *
  * md5sum is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should hav received a copy of the GNU General Public License
  * along with md5sum; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  */

#include "md5.h"

void
md5_digest (const char *data __UU__, unsigned int length __UU__, unsigned char *result __UU__)
{
  
}

int
md5_digest_file (const char *filename, unsigned char *result)
{
  int fd = open(filename, O_RDONLY);
  assert_inner(fd != -1, "open");

  struct stat sb;
  int res = fstat(fd, &sb);
  assert_inner(res != -1, "fstat");

  char *data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  assert_inner(data != MAP_FAILED, "mmap");

  md5_digest(data, sb.st_size, result);

  munmap(data, sb.st_size);

  return 0;
}
