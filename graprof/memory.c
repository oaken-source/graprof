
#include "memory.h"

#define _ __attribute__ ((unused))

int
memory_malloc (_ size_t size, _ uintptr_t caller, _ uintptr_t result, _ unsigned long long time)
{

  return 0;
}

int
memory_realloc (_ uintptr_t ptr, _ size_t size, _ uintptr_t caller, _ uintptr_t result, _ unsigned long long time)
{

  return 0;
}

int memory_free (_ uintptr_t ptr, _ uintptr_t caller, _ unsigned long long time)
{

  return 0;
}
