
#include "function.h"
#include "timeline.h"
#include "addr.h"

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <grapes/util.h>
#include <grapes/feedback.h>

static function *functions = NULL;
static unsigned int nfunctions = 0;

struct stack_entry
{
  unsigned int function_id;
  unsigned long long entry_time;
};

typedef struct stack_entry stack_entry;

static stack_entry *call_stack = NULL;
static unsigned int stack_size = 0;

static function* 
function_push (uintptr_t address)
{
  ++nfunctions;
  functions = realloc(functions, sizeof(*functions) * nfunctions);
  assert_inner_ptr(functions, "realloc");

  function *f = functions + nfunctions - 1;

  f->address = address;
  f->name = NULL;
  f->file = NULL;
  f->line = 0;
 
  addr_translate(f->address, &(f->name), &(f->file), &(f->line));

  feedback_assert_wrn(strcmp(f->name, "??"), "unable to identify instrumented function at 0x%" PRIxPTR " - missing debug symbols?", address);

  f->self_time = 0;
  f->cumulative_time = 0;
  f->calls = 0;
 
  return f;
}

int
function_enter (uintptr_t address, unsigned long long time)
{
  function *f = function_get_by_address(address);
  if (!f)
    f = function_push(address);
  assert_inner(f, "function_push");
 
  ++(f->calls);

  ++stack_size;
  call_stack = realloc(call_stack, sizeof(*call_stack) * stack_size);
  call_stack[stack_size - 1].function_id = f - functions;
  call_stack[stack_size - 1].entry_time = time;

  return 0;
}

int
function_exit (uintptr_t address, unsigned long long time __attribute__ ((unused)))
{
  function *f = function_get_by_address(address);

  if (!f)
    {
      feedback_warning("0x%" PRIxPTR ": exting unknown function - skewed trace data?");
      return 0;
    }

  if (!stack_size || functions[call_stack[stack_size - 1].function_id].address != address)
    {
      feedback_warning("0x%" PRIxPTR ": exiting function not on top of the call stack - skewed trace data?");
      return 0;
    }

  --stack_size;

  return 0;
}

int 
function_exit_all (unsigned long long time)
{
  while (stack_size)
    {
      uintptr_t func = functions[call_stack[stack_size - 1].function_id].address;
      int res = function_exit(func, time);
      assert_inner(!res, "function_exit");
    }

  if (call_stack)
    free(call_stack);

  return 0;
}

function*
function_get_by_address (uintptr_t address)
{
  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
    if (functions[i].address == address)
      return (functions + i);

  return NULL;
}

#define FUNCTION_MAX_ADDR_DISTANCE 0xFFFF

function*
function_get_by_nearest_address (uintptr_t address)
{
  function *best = NULL;

  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
    if (functions[i].address < address && address - functions[i].address < FUNCTION_MAX_ADDR_DISTANCE)
      if (!best || functions[i].address > address)
        best = functions + i;

  return best;
}

function*
function_get_all(unsigned int *nfunctions_ptr)
{
  *nfunctions_ptr = nfunctions;
  return functions;
}
