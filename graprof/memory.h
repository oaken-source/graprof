
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013  Andreas Grapentin                                   *
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

#include "function.h"

#include <stdlib.h>
#include <stdint.h>

#define FAILED_RESULT      0x01
#define FAILED_INVALID_PTR 0x02
#define FAILED_DOUBLE_FREE 0x03


struct failed_malloc
{
  size_t size;
  function *caller;
  unsigned int direct_call;
  unsigned long long time;

  char *file;
  unsigned int line;
};

typedef struct failed_malloc failed_malloc;

struct failed_realloc
{
  unsigned int reason;
  size_t start_size;
  size_t end_size;
  uintptr_t ptr;
  function *caller;
  unsigned int direct_call;
  unsigned long long time;

  char *file;
  unsigned int line;
};

typedef struct failed_realloc failed_realloc;

struct failed_free
{
  unsigned int reason;
  uintptr_t ptr;
  function *caller;
  unsigned int direct_call;
  unsigned long long time;

  char *file;
  unsigned int line;
};

typedef struct failed_free failed_free;

int memory_malloc(size_t, uintptr_t, uintptr_t, unsigned long long);

int memory_realloc(uintptr_t, size_t, uintptr_t, uintptr_t, unsigned long long);

int memory_free(uintptr_t, uintptr_t, unsigned long long);

unsigned long long memory_get_total_allocated();
unsigned long long memory_get_maximum_allocated();
unsigned long long memory_get_total_freed();

unsigned int memory_get_total_allocations();
unsigned int memory_get_total_reallocations();
unsigned int memory_get_total_frees();

failed_malloc* memory_get_failed_mallocs(unsigned int *nfailed_mallocs);
failed_realloc* memory_get_failed_reallocs(unsigned int *nfailed_reallocs);
failed_free* memory_get_failed_frees(unsigned int *nfailed_frees);
