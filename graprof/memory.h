
#pragma once

#include <stdlib.h>
#include <stdint.h>

int memory_malloc(size_t, uintptr_t, uintptr_t, unsigned long long);

int memory_realloc(uintptr_t, size_t, uintptr_t, uintptr_t, unsigned long long);

int memory_free(uintptr_t, uintptr_t, unsigned long long);

unsigned long long memory_get_total_allocated();
unsigned long long memory_get_maximum_allocated();
unsigned long long memory_get_total_freed();

unsigned int memory_get_total_allocations();
unsigned int memory_get_total_reallocations();
unsigned int memory_get_total_frees();


