
#include "memory.h"

#include <stdio.h>
#include <inttypes.h>

#include <grapes/util.h>

#define _ __attribute__ ((unused))

unsigned long long total_allocated = 0;
unsigned long long maximum_allocated = 0;
unsigned long long total_freed = 0;

unsigned int total_allocations = 0;
unsigned int total_reallocations = 0;
unsigned int total_frees = 0;

struct block
{
  uintptr_t address;
  size_t size;
  unsigned int freed;
};

typedef struct block block;

block *blocks = NULL;
unsigned int nblocks = 0;

static block*
memory_get_block_by_address (uintptr_t address)
{
  unsigned int i;
  for (i = 0; i < nblocks; ++i)
    if (blocks[i].address == address)
      return blocks + i;

  return NULL;
}

int
memory_malloc (size_t size, _ uintptr_t caller, uintptr_t result, _ unsigned long long time)
{
  if (!result)
    {
      // TODO: memory_add_failed_malloc(size, caller, result, time, FAILED_RESULT);
      return 0;
    }

  total_allocated += size;

  ++total_allocations;

  if (total_allocated > maximum_allocated)
    maximum_allocated = total_allocated;

  ++nblocks;
  blocks = realloc(blocks, sizeof(*blocks) * nblocks);
  assert_inner(blocks, "realloc");

  blocks[nblocks - 1].address = result;
  blocks[nblocks - 1].size = size;
  blocks[nblocks - 1].freed = 0;

  return 0;
}

int
memory_realloc (uintptr_t ptr, size_t size, uintptr_t caller, uintptr_t result, unsigned long long time)
{
  // edge cases: 
  //   if ptr == NULL, behave like malloc but keep correct counters
  if (!ptr)
    {
      ++total_reallocations;
      --total_allocations;
      memory_malloc(size, caller, result, time);
      return 0;
    }

  //   if size == 0, behave like free but keep correct counters
  if (!size)
    {
      ++total_reallocations;
      --total_frees;
      memory_free(ptr, caller, time);
      return 0;
    }

  if (!result)
    {
      // TODO: memory_add_failed_realloc(ptr, size, caller, result, time, FAILED_RESULT);
      return 0;
    }

  block *b = memory_get_block_by_address(ptr);
  if (!b || b->freed)
    {
      // TODO: memory_add_failed_realloc(ptr, size, caller, result, time, FAILED_INVALID_PTR);
      return 0;
    }

  ++total_reallocations;

  int diff = size - b->size;
  if (diff > 0)
    total_allocated += diff;

  if (total_allocated > maximum_allocated)
    maximum_allocated = total_allocated;

  b->address = result;
  b->size = size;

  return 0;
}

int memory_free (uintptr_t ptr, _ uintptr_t caller, _ unsigned long long time)
{
  block *b = memory_get_block_by_address(ptr);
  if (!b)
    {
      // TODO: memory_add_failed_free(ptr, caller, time, FAILED_INVALID_PTR);
      return 0;
    }

  if (b->freed)
    {
      // TODO: memory_add_failed_free(ptr, caller, time, FAILED_DOUBLE_FREE);
      return 0;
    }

  b->freed = 1;
  total_freed += b->size;
  ++total_frees;

  return 0;
}

/* simple accessors */

unsigned long long
memory_get_total_allocated ()
{
  return total_allocated;
}

unsigned long long
memory_get_maximum_allocated ()
{
  return maximum_allocated;
}

unsigned long long
memory_get_total_freed ()
{
  return total_freed;
}

unsigned int
memory_get_total_allocations ()
{
  return total_allocations;
}

unsigned int
memory_get_total_reallocations ()
{
  return total_reallocations;
}

unsigned int
memory_get_total_frees ()
{
  return total_frees;
}
