
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

#include <config.h>

#include "function.h"

#include <stdlib.h>
#include <stdint.h>

/* some types of memory allocation / deallocation failures
 */
#define FAILED_RESULT      0x01
#define FAILED_INVALID_PTR 0x02

/* this struct holds all information relevant to a malloc failure
 */
struct failed_malloc
{
  size_t size;
  function *caller;
  unsigned int direct_call;
  unsigned long long time;

  char *file;
  unsigned int line;
  char *func;
};

typedef struct failed_malloc failed_malloc;

/* this struct holds all information relevant to a realloc failure
 */
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
  char *func;
};

typedef struct failed_realloc failed_realloc;

/* this struct holds all information relevant to a free failure
 */
struct failed_free
{
  unsigned int reason;
  uintptr_t ptr;
  function *caller;
  unsigned int direct_call;
  unsigned long long time;

  char *file;
  unsigned int line;
  char *func;
};

typedef struct failed_free failed_free;

/* process an allocation event that has been read from the trace data. This
 * event may have been gathered from calls to malloc or calloc
 *
 * params:
 *   size - the size of the allocation in bytes
 *   caller - the return address of the malloc call - 4
 *   result - the return value of the malloc call
 *   time - the time of the malloc call in ns
 *
 * errors:
 *   may fail and set errno for the same reasons as realloc and addr_translate
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int memory_malloc(size_t size, uintptr_t caller, uintptr_t result, unsigned long long time);

/* process a reallocation event that has been read from the trace data. Events
 * that fulfill certain conditions are relayed as allocation or free events.
 *
 * params:
 *   ptr - the pointer passed to realloc
 *   size - the size of the reallocation in bytes
 *   caller - the return address of the realloc call - 4
 *   result - the return value of the realloc call
 *   time - the time of the realloc call in ns
 *
 * errors:
 *   may fail and set errno for the same reasons as realloc and addr_translate
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int memory_realloc(uintptr_t ptr, size_t size, uintptr_t caller, uintptr_t result, unsigned long long time);

/* process a free event that has been read from the trace data
 *
 * params:
 *   ptr - the pointer passed to free
 *   caller - the return address of the free call - 4
 *   time - the time of the free call in ns
 *
 * errors:
 *   may fail and set errno for the same reasons as realloc and addr_translate
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int memory_free(uintptr_t ptr, uintptr_t caller, unsigned long long time);

/* get the total number of bytes allocated
 *
 * returns:
 *   the total number of bytes allocated
 */
unsigned long long memory_get_total_allocated(void);

/* get the maximum number of bytes allocated during child runtime
 *
 * returns:
 *   the maximum number of bytes allocated
 */
unsigned long long memory_get_maximum_allocated(void);

/* get the total number of bytes freed
 *
 * returns:
 *   the total number of bytes freed
 */
unsigned long long memory_get_total_freed(void);

/* get the total number of allocation trace events
 *
 * returns:
 *   the total number of allocations
 */
unsigned int memory_get_total_allocations(void);

/* get the total number of reallocation trace events
 *
 * returns:
 *   the total number of reallocation events
 */
unsigned int memory_get_total_reallocations(void);

/* get the total number of deallocation trace events
 *
 * returns:
 *   the total number of deallocation events
 */
unsigned int memory_get_total_frees(void);

/* get a list of failed allocation events and their meta information
 *
 * params:
 *   nfailed_mallocs - pointer where the size of the returned array is stored
 *
 * returns:
 *   a pointer to the first element in an array of failed allocation events
 */
failed_malloc* memory_get_failed_mallocs(unsigned int *nfailed_mallocs);

/* get a list of failed reallocation events and their meta information
 *
 * params:
 *   nfailed_reallocs - pointer where the size of the returned array is stored
 *
 * returns:
 *   a pointer to the first element in an array of failed reallocation events
 */
failed_realloc* memory_get_failed_reallocs(unsigned int *nfailed_reallocs);

/* get a list of failed deallocation events and their meta information
 *
 * params:
 *   nfailed_freed - pointer where the size of the returned array is stored
 *
 * returns:
 *   a pointer to the first element in an array of failed deallocation events
 */
failed_free* memory_get_failed_frees(unsigned int *nfailed_frees);

