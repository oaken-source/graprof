
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

#include <stdint.h>

/* this struct represents a trace event pushed to the trace log
 */
struct tracebuffer_packet
{
  char type;

  union {
    struct {
      uintptr_t func;
      uintptr_t caller;
    } enter;
    struct {
    } exit;
    struct {
      size_t size;
      uintptr_t caller;
      uintptr_t result;
    } malloc;
    struct {
      size_t size;
      uintptr_t caller;
      uintptr_t result;
    } calloc;
    struct {
      uintptr_t ptr;
      size_t size;
      uintptr_t caller;
      uintptr_t result;
    } realloc;
    struct {
      uintptr_t ptr;
      uintptr_t caller;
    } free;
  };

  unsigned long long time;
};
typedef struct tracebuffer_packet tracebuffer_packet;

/* open a tracebuffer file for writing
 *
 * params:
 *   filename - the name of the tracebuffer file
 *
 * errors:
 *   may fail and set errno for the same reasons as fopen and fwrite
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int tracebuffer_openw (const char *filename) __may_fail;

/* append an object file entry to the header of the currently opened
 * tracebuffer file
 *
 * params:
 *   filename - the name of the object file to enter
 *   offset - the load address of the object file in memory
 *
 * errors:
 *   may fail and set errno for the same reasons as digest_file and fwrite
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int tracebuffer_append_header (const char *filename, uintptr_t offset) __may_fail;

/* mark the end of the header of the currently written tracebuffer file header
 *
 * errors:
 *   may fail and set errno for the same reasons as fseek and fwrite
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int tracebuffer_finish_header (void) __may_fail;

/* add a packet to the currently active tracebuffer file
 *
 * params:
 *   p - a pointer to a tracebuffer_packet
 *
 * errors:
 *   may fail and set errno for the same reasons as fwrite
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int tracebuffer_append (const tracebuffer_packet *p) __may_fail;

/* close the currently active tracebuffer file and flush all pending writes.
 * if this is not called, the written tracebuffer will be incomplete
 *
 * errors:
 *   may fail and set errno for the same reasons as fwrite and fclose
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int tracebuffer_close (void) __may_fail;

/* iterate over the entries in the given tracefiles's header ad call the given
 * callback for each.
 * will stop iterating if one of the callback invocations does not return 0
 *
 * params:
 *   callback - the function to call for each header entry
 *   filename - the name of the tracebuffer file
 *
 * errors:
 *   may fail and set errno for the same reasons as fopen, fclose, fread,
 *   ftell, digest_file and the given callback.
 *   will also fail and set errno to ENOTSUP if the given file is no valid
 *   tracebuffer file.
 *   will emit a warning to stderr if the given header entry has changed on
 *   disk (the digest does not match the one stored in the header)
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int tracebuffer_iterate_header (int(*callback)(const char*, uintptr_t), const char *filename) __may_fail;

/* iterate over the entries in the given tracefile's payload and call the
 * given callback for each.
 * will stop iterating if one of the callback invocations does not return 0
 *
 * params:
 *   callback - the function to call for each header entry
 *   filename - the name of the tracebuffer file
 *
 * errors:
 *   may fail and set errno for the same reasons as file_map, file_unmap and
 *   the given callback.
 *   the behaviour is undefined, if tracebuffer_iterate_header has not been
 *   called.
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int tracebuffer_iterate_packet (int(*callback)(tracebuffer_packet*, int), const char *filename) __may_fail;
