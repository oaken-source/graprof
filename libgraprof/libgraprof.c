
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


#define _GNU_SOURCE

#include "libgraprof.h"

#include "tracebuffer.h"

#include "common/md5.h"

#include <grapes/feedback.h>

#include <link.h>

#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

FILE *libgraprof_out = NULL;
char *libgraprof_filename = NULL;

unsigned int libgraprof_hooked = 0;

static int __may_fail
libgraprof_count_object_files (struct dl_phdr_info *info, __unused size_t size, void *data)
{
  uint32_t *count = data;

  char path[PATH_MAX] = { 0 };

  if (!info->dlpi_addr)
    return 0;
  if (!realpath(info->dlpi_name, path))
    return 0;

  ++(*count);

  return 0;
}

static int __may_fail
libgraprof_write_header_entry(const char *filename, uintptr_t offset)
{
  size_t res = fwrite(&offset, sizeof(offset), 1, libgraprof_out);
  __checked_call(res == 1);

  unsigned char digest[DIGEST_LENGTH];
  __checked_call(0 == md5_digest(digest, filename));

  res = fwrite(digest, 1, DIGEST_LENGTH, libgraprof_out);
  __checked_call(res == DIGEST_LENGTH);

  size_t length = strlen(filename) + 1;
  res = fwrite(&length, sizeof(length), 1, libgraprof_out);
  __checked_call(res == 1);

  res = fwrite(filename, 1, length, libgraprof_out);
  __checked_call(res == length);

  return 0;
}

static int __may_fail
libgraprof_write_header_for_object_file (struct dl_phdr_info *info, __unused size_t size, __unused void *data)
{
  char path[PATH_MAX] = { 0 };

  if (!info->dlpi_addr)
    return 0;
  if (!realpath(info->dlpi_name, path))
    return 0;

  __checked_call(0 == libgraprof_write_header_entry(path, info->dlpi_addr));

  return 0;
}

static int __may_fail
libgraprof_write_header (void)
{
  size_t res = fwrite(TRACEBUFFER_MAGIC_NUMBER, 1, sizeof(TRACEBUFFER_MAGIC_NUMBER), libgraprof_out);
  __checked_call(res == sizeof(TRACEBUFFER_MAGIC_NUMBER));

  char *filename;
  __checked_call(NULL != (filename = realpath("/proc/self/exe", NULL)));

  uint32_t count = 1;
  dl_iterate_phdr(libgraprof_count_object_files, &count);

  res = fwrite(&count, sizeof(count), 1, libgraprof_out);
  __checked_call(res == 1);

  __checked_call(0 == libgraprof_write_header_entry(filename, 0));
  __checked_call(0 == dl_iterate_phdr(libgraprof_write_header_for_object_file, NULL));

  return 0;
}

static void
__attribute__ ((constructor))
libgraprof_init ()
{
  int errnum = errno;

  libgraprof_filename = getenv("GRAPROF_OUT");

  if (libgraprof_filename)
    {
      unlink(libgraprof_filename);
      libgraprof_out = fopen(libgraprof_filename, "wb");
      feedback_assert_wrn(libgraprof_out, "libgraprof: `%s'", libgraprof_filename);
    }

  if (libgraprof_out)
    {
      int res = libgraprof_write_header();
      feedback_assert_wrn(!res, "libgraprof: `%s:' unable to write trace header", libgraprof_filename);
      libgraprof_install_hooks();
    }

  errno = errnum;
}

static void
__attribute__ ((destructor))
libgraprof_fini ()
{
  int errnum = errno;

  if (libgraprof_out)
  {
    libgraprof_uninstall_hooks();

    static tracebuffer_packet p = { .type = 'E' };
    tracebuffer_append(&p);
    tracebuffer_finish();

    int res = fclose(libgraprof_out);
    feedback_assert_wrn(!res, "libgraprof: unable to close '%s'", libgraprof_filename);
  }

  errno = errnum;
}
