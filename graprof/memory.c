
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


#include "memory.h"
#include "blocklist.h"
#include "addr.h"

#include <stdio.h>
#include <inttypes.h>

#include <grapes/util.h>

unsigned long long total_allocated = 0;
unsigned long long current_allocated = 0;
unsigned long long maximum_allocated = 0;
unsigned long long total_freed = 0;

unsigned int total_allocations = 0;
unsigned int total_reallocations = 0;
unsigned int total_frees = 0;

failed_malloc *failed_mallocs = NULL;
unsigned int nfailed_mallocs = 0;

failed_realloc *failed_reallocs = NULL;
unsigned int nfailed_reallocs = 0;

failed_free *failed_frees = NULL;
unsigned int nfailed_frees = 0;

uintptr_t *freed_addresses = NULL;
unsigned int nfreed_addresses = 0;


static int
memory_add_failed_malloc (size_t size, uintptr_t caller, unsigned long long time)
{
  ++nfailed_mallocs;
  failed_mallocs = realloc(failed_mallocs, sizeof(*failed_mallocs) * nfailed_mallocs);
  assert_inner(failed_mallocs, "realloc");

  failed_malloc *f = failed_mallocs + nfailed_mallocs - 1;

  f->size = size;
  f->time = time;
  f->file = NULL;
  f->line = 0;
  f->func = NULL;

  function *func = function_get_current();

  f->caller = func;

  if (!function_compare(func, caller))
    {
      f->direct_call = 1;
      int res = addr_translate(caller, &(f->func), &(f->file), &(f->line));
      assert_inner(!res, "addr_translate");
    }
  else
    {
      f->direct_call = 0;
      f->file = func->file;
      f->line = func->line;
      f->func = func->name;
    }

  return 0;
}

static int
memory_add_failed_realloc (uintptr_t ptr, size_t size, uintptr_t caller, unsigned long long time, unsigned int reason)
{
  ++nfailed_reallocs;
  failed_reallocs = realloc(failed_reallocs, sizeof(*failed_reallocs) * nfailed_reallocs);
  assert_inner(failed_reallocs, "realloc");

  failed_realloc *f = failed_reallocs + nfailed_reallocs - 1;

  f->reason = reason;

  f->start_size = 0;
  if (f->reason != FAILED_INVALID_PTR)
    {
      block *b = blocklist_get_by_address(ptr);
      f->start_size = b->size;
    }

  f->end_size = size;
  f->ptr = ptr;
  f->time = time;
  f->file = NULL;
  f->line = 0;
  f->func = NULL;

  function *func = function_get_current();

  f->caller = func;

  if (!function_compare(func, caller))
    {
      f->direct_call = 1;
      int res = addr_translate(caller, &(f->func), &(f->file), &(f->line));
      assert_inner(!res, "addr_reanslate");
    }
  else
    {
      f->direct_call = 0;
      f->file = func->file;
      f->line = func->line;
      f->func = func->name;
    }

  return 0;
}

static int
memory_add_failed_free (uintptr_t ptr, uintptr_t caller, unsigned long long time, unsigned int reason)
{
  ++nfailed_frees;
  failed_frees = realloc(failed_frees, sizeof(*failed_frees) * nfailed_frees);
  assert_inner(failed_frees, "realloc");

  failed_free *f = failed_frees + nfailed_frees - 1;

  f->reason = reason;

  f->ptr = ptr;
  f->time = time;
  f->file = NULL;
  f->line = 0;
  f->func = NULL;

  function *func = function_get_current();

  f->caller = func;

  if (!function_compare(func, caller))
    {
      f->direct_call = 1;
      int res = addr_translate(caller, &(f->func), &(f->file), &(f->line));
      assert_inner(!res, "addr_translate");
    }
  else
    {
      f->direct_call = 0;
      f->file = func->file;
      f->line = func->line;
      f->func = func->name;
    }

  return 0;
}

int
memory_malloc (size_t size, uintptr_t caller, uintptr_t result, unsigned long long time)
{
  ++total_allocations;

  if (!result)
    {
      int res = memory_add_failed_malloc(size, caller, time);
      assert_inner(!res, "memory_add_failed_malloc");
      return 0;
    }

  total_allocated += size;
  current_allocated += size;

  if (current_allocated > maximum_allocated)
    maximum_allocated = current_allocated;

  block *b = blocklist_add(result);
  assert_inner(b, "blocklist_add");
  b->size = size;

  function *func = function_get_current();

  if (!function_compare(func, caller))
    {
      b->direct_call = 1;
      int res = addr_translate(caller, &(b->func), &(b->file), &(b->line));
      assert_inner(!res, "addr_reanslate");
    }
  else
    {
      b->direct_call = 0;
      b->file = func->file;
      b->line = func->line;
      b->func = func->name;
    }

  return 0;
}

int
memory_realloc (uintptr_t ptr, size_t size, uintptr_t caller, uintptr_t result, unsigned long long time)
{
  ++total_reallocations;

  // edge cases:
  //   if ptr == NULL, behave like malloc but keep correct counters
  if (!ptr)
    {
      int res = memory_malloc(size, caller, result, time);
      assert_inner(!res, "memory_malloc");
      --total_allocations;
      return 0;
    }

  //   if size == 0, behave like free but keep correct counters
  if (!size)
    {
      int res = memory_free(ptr, caller, time);
      assert_inner(!res, "memory_free");
      --total_frees;
      return 0;
    }

  block *b = blocklist_get_by_address(ptr);
  if (!b)
    {
      int res = memory_add_failed_realloc(ptr, size, caller, time, FAILED_INVALID_PTR);
      assert_inner(!res, "memory_add_failed_realloc");
      return 0;
    }

  if (!result)
    {
      int res = memory_add_failed_realloc(ptr, size, caller, time, FAILED_RESULT);
      assert_inner(!res, "memory_add_failed_realloc");
      return 0;
    }

  int diff = size - b->size;
  if (diff > 0)
    total_allocated += diff;
  else
    total_freed -= diff;

  current_allocated += diff;

  if (current_allocated > maximum_allocated)
    maximum_allocated = current_allocated;

  b->size = size;
  if (b->address != result)
    blocklist_relocate(b, result);
    
  return 0;
}

int memory_free (uintptr_t ptr, uintptr_t caller, unsigned long long time)
{
  ++total_frees;

  block *b = blocklist_get_by_address(ptr);
  if (!b)
    {
      int res = memory_add_failed_free(ptr, caller, time, FAILED_INVALID_PTR);
      assert_inner(!res, "memory_add_failed_free");
      return 0;
    }

  total_freed += b->size;
  current_allocated -= b->size;

  blocklist_remove(b);

  return 0;
}

/* simple accessors */

unsigned long long
memory_get_total_allocated (void)
{
  return total_allocated;
}

unsigned long long
memory_get_maximum_allocated (void)
{
  return maximum_allocated;
}

unsigned long long
memory_get_total_freed (void)
{
  return total_freed;
}

unsigned int
memory_get_total_allocations (void)
{
  return total_allocations;
}

unsigned int
memory_get_total_reallocations (void)
{
  return total_reallocations;
}

unsigned int
memory_get_total_frees (void)
{
  return total_frees;
}

failed_malloc*
memory_get_failed_mallocs (unsigned int *n)
{
  *n = nfailed_mallocs;
  return failed_mallocs;
}

failed_realloc*
memory_get_failed_reallocs (unsigned int *n)
{
  *n = nfailed_reallocs;
  return failed_reallocs;
}

failed_free*
memory_get_failed_frees (unsigned int *n)
{
  *n = nfailed_frees;
  return failed_frees;
};

static void
__attribute__((destructor))
memory_fini (void)
{
  unsigned int i;
  for (i = 0; i < nfailed_mallocs; ++i)
    if (failed_mallocs[i].direct_call)
      {
        free(failed_mallocs[i].file);
        free(failed_mallocs[i].func);
      }
  free(failed_mallocs);

  for (i = 0; i < nfailed_reallocs; ++i)
    if (failed_reallocs[i].direct_call)
      {
        free(failed_reallocs[i].file);
        free(failed_reallocs[i].func);
      }
  free(failed_reallocs);

  for (i = 0; i < nfailed_frees; ++i)
    if (failed_frees[i].direct_call)
      {
        free(failed_frees[i].file);
        free(failed_frees[i].func);
      }
  free(failed_frees);
}

