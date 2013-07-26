
#pragma once

#include <stdint.h>

struct call_data
{
  unsigned int function_id;
  unsigned long calls;
};

typedef struct call_data call_data;

struct function
{
  uintptr_t address;
  char *name;
  char *file;
  unsigned int line;

  unsigned long long self_time;
  unsigned long long cumulative_time;
  unsigned long calls;

  call_data *callers;
  unsigned int ncallers;
  call_data *callees;
  unsigned int ncallees;
};

typedef struct function function;

int function_enter(uintptr_t, uintptr_t, unsigned long long);

int function_exit(unsigned long long);

int function_exit_all(unsigned long long);

function* function_get_by_address(uintptr_t);

function* function_get_all(unsigned int*);
