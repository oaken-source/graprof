
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

#include "common/md5.h"

#include <stdint.h>

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
      unsigned char digest[DIGEST_LENGTH];
    } exit_all;
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

void tracebuffer_append (tracebuffer_packet p);

void tracebuffer_finish (void);
