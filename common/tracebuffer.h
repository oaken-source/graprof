
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
    struct {
    } exit_all;
  };

  unsigned long long time;
};
typedef struct tracebuffer_packet tracebuffer_packet;

int tracebuffer_openw (const char *filename) __may_fail;

int tracebuffer_append_header (const char *filename, uintptr_t offset) __may_fail;

int tracebuffer_finish_header (void);

/* add a packet to the trace log
 *
 * params:
 *   p - a pointer to a tracebuffer_packet
 *
 * errors:
 *   will emit a warning to stderr if a write operation fails
 */
int tracebuffer_append (const tracebuffer_packet *p) __may_fail;

/* finish a trace log - should be called after all packets are appended, to
 * flush the internal write buffer
 *
 * errors:
 *   will emit a warning to stderr if a write operation fails
 */
int tracebuffer_close (void) __may_fail;

int tracebuffer_iterate_header (int(*callback)(const char*, uintptr_t), const char *filename) __may_fail;

int tracebuffer_iterate_packet (int(*callback)(tracebuffer_packet*, int), const char *filename) __may_fail;
