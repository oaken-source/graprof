
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


#include "tracebuffer.h"

#include "digest.h"

#include <grapes/feedback.h>
#include <grapes/file.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 1024

static tracebuffer_packet tracebuffer_buffer[BUFSIZE];
size_t tracebuffer_index = 0;

static FILE *tracebuffer_file;

static unsigned char MAGIC_NUMBER[] = { 0x74, 0xCE, 0xB0, 0xFF };

int
tracebuffer_openw (const char *filename)
{
  unlink(filename);
  __checked_call(NULL != (tracebuffer_file = fopen(filename, "wb")));

  size_t res = fwrite(MAGIC_NUMBER, 1, sizeof(MAGIC_NUMBER), tracebuffer_file);
  __checked_call(res == sizeof(MAGIC_NUMBER));

  size_t count = 0;
  res = fwrite(&count, sizeof(count), 1, tracebuffer_file);
  __checked_call(res == 1);

  return 0;
}

static size_t tracebuffer_header_count = 0;

int
tracebuffer_append_header (const char *filename, uintptr_t offset)
{
  size_t res = fwrite(&offset, sizeof(offset), 1, tracebuffer_file);
  __checked_call(res == 1);

  digest_t digest;
  __checked_call(0 == digest_file(&digest, filename));
  res = fwrite(digest, 1, sizeof(digest), tracebuffer_file);
  __checked_call(res == sizeof(digest));

  size_t length = strlen(filename) + 1;
  res = fwrite(&length, sizeof(length), 1, tracebuffer_file);
  __checked_call(res == 1);

  res = fwrite(filename, 1, length, tracebuffer_file);
  __checked_call(res == length);

  ++tracebuffer_header_count;

  return 0;
}

int
tracebuffer_finish_header (void)
{
  __checked_call(0 == fseek(tracebuffer_file, sizeof(MAGIC_NUMBER), SEEK_SET));

  size_t res = fwrite(&tracebuffer_header_count, sizeof(tracebuffer_header_count), 1, tracebuffer_file);
  __checked_call(res == 1);

  __checked_call(0 == fseek(tracebuffer_file, 0, SEEK_END));

  return 0;
}

static int
tracebuffer_flush (void)
{
  size_t res = fwrite(tracebuffer_buffer, sizeof(*tracebuffer_buffer), tracebuffer_index, tracebuffer_file);
  __checked_call(res == tracebuffer_index);

  tracebuffer_index = 0;

  return 0;
}

int
tracebuffer_append (const tracebuffer_packet *p)
{
  tracebuffer_buffer[tracebuffer_index++] = *p;

  if (tracebuffer_index == BUFSIZE)
    __checked_call(0 == tracebuffer_flush());

  return 0;
}

int
tracebuffer_close (void)
{
  if (tracebuffer_index)
    tracebuffer_flush();

  __checked_call(0 == fclose(tracebuffer_file));

  return 0;
}

static long tracebuffer_packets_offset;

int
tracebuffer_iterate_header_inner (int(*callback)(const char*, uintptr_t), FILE *in)
{
  unsigned char magic[sizeof(MAGIC_NUMBER)];
  size_t res = fread(magic, 1, sizeof(MAGIC_NUMBER), in);
  __checked_call(res == sizeof(MAGIC_NUMBER));

  __precondition(ENOTSUP, !memcmp(magic, MAGIC_NUMBER, sizeof(MAGIC_NUMBER)));

  size_t count;
  res = fread(&count, sizeof(count), 1, in);
  __checked_call(res == 1);

  size_t i;
  for (i = 0; i < count; ++i)
    {
      uintptr_t offset;
      res = fread(&offset, sizeof(offset), 1, in);
      __checked_call(res == 1);

      digest_t d1;
      res = fread(d1, 1, sizeof(d1), in);
      __checked_call(res == sizeof(d1));

      size_t length;
      res = fread(&length, sizeof(length), 1, in);
      __checked_call(res == 1);

      char *filename = alloca(length);
      res = fread(filename, 1, length, in);
      __checked_call(res == length);

      digest_t d2;
      __checked_call(0 == digest_file(&d2, filename));

      feedback_assert_wrn(!digest_cmp(d1, d2), "`%s': has changed on disk", filename);

      __checked_call(0 == callback(filename, offset));
    }

  __checked_call(0 <= (tracebuffer_packets_offset = ftell(in)));

  return 0;
}

int
tracebuffer_iterate_header (int(*callback)(const char*, uintptr_t), const char *filename)
{
  FILE *in;
  __checked_call(NULL != (in = fopen(filename, "rb")));

  __checked_section
    (
      __section_call(0 == tracebuffer_iterate_header_inner(callback, in));
    )

  __checked_call(0 == fclose(in));

  __on_failure
    (
      return -1;
    )

  return 0;
}

int
tracebuffer_iterate_packet_inner (int(*callback)(tracebuffer_packet*, int), void *data, size_t length)
{
  data += tracebuffer_packets_offset;
  length -= tracebuffer_packets_offset;

  tracebuffer_packet *packets = data;
  size_t npackets = length / sizeof(*packets);

  size_t i;
  for (i = 0; i < npackets; ++i)
    __checked_call(0 == callback(packets + i, i == npackets - 1));

  return 0;
}

int
tracebuffer_iterate_packet (int(*callback)(tracebuffer_packet*, int), const char *filename)
{
  size_t length;
  void *data;

  __checked_call(NULL != (data = file_map(filename, &length)));

  __checked_call(0 == tracebuffer_iterate_packet_inner(callback, data, length),
    __checked_call(0 == file_unmap(data, length));
  );

  __checked_call(0 == file_unmap(data, length));

  return 0;
}
