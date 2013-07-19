
#pragma once

#include <stdint.h>

int addr_init(const char*);

int addr_translate(uintptr_t, char**, char**, unsigned int*);
