
#pragma once

#include <stdlib.h>
#include <stdint.h>

int memory_malloc(size_t, uintptr_t, uintptr_t, unsigned long long);

int memory_realloc(uintptr_t, size_t, uintptr_t, uintptr_t, unsigned long long);

int memory_free(uintptr_t, uintptr_t, unsigned long long);
