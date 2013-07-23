
#include "function.h"
//#include "timeline.h"
#include "addr.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <grapes/util.h>
#include <grapes/feedback.h>

static function *functions = NULL;
static unsigned int nfunctions = 0;

struct tree_entry
{
  unsigned int function_id;
  unsigned long long entry_time;
  unsigned long long exit_time;

  uintptr_t caller;

  struct tree_entry *parent;
  struct tree_entry *children;
  unsigned int nchildren;
};

typedef struct tree_entry tree_entry;

static tree_entry call_tree_root = { -1, 0, 0, 0, NULL, NULL, 0 };
static tree_entry *call_tree_current_node = &call_tree_root;

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

static unsigned long long
function_aggregate_recursive_for_id (tree_entry *t, unsigned int id)
{
  unsigned long long time = 0;
  unsigned int i;
  for (i = 0; i < t->nchildren; ++i)
  {
    if (t->children[i].function_id == id)
      time += t->children[i].exit_time - t->children[i].entry_time;
    else
      time += function_aggregate_recursive_for_id(t->children + i, id);
  }

  return time;
}

static void
function_aggregate_recursive (tree_entry *t)
{
  unsigned long long children_time = 0;
  unsigned int i;
  for (i = 0; i < t->nchildren; ++i)
    {
      function_aggregate_recursive(t->children + i);
      children_time += t->children[i].exit_time - t->children[i].entry_time;
    }

  if (t->function_id != (unsigned int)-1)
    {
      unsigned long long children_self_time = function_aggregate_recursive_for_id(t, t->function_id);

      //printf("I am %s [0x%" PRIxPTR "] %llu-%llu (%llu, %llu)\n", functions[t->function_id].name, functions[t->function_id].address, t->entry_time, t->exit_time, children_time, children_self_time);

      functions[t->function_id].self_time += t->exit_time - t->entry_time - children_time;
      functions[t->function_id].cumulative_time += t->exit_time - t->entry_time - children_self_time;
    }
}

int
function_enter (uintptr_t address, uintptr_t caller, unsigned long long time)
{
  function *f = function_get_by_address(address);
  if (!f)
    f = function_push(address);
  assert_inner(f, "function_push");
 
  ++(f->calls);

  ++(call_tree_current_node->nchildren);
  call_tree_current_node->children = realloc(call_tree_current_node->children, sizeof(tree_entry) * call_tree_current_node->nchildren);
  assert_inner(call_tree_current_node->children, "realloc");

  tree_entry *next_node = call_tree_current_node->children + call_tree_current_node->nchildren - 1;

  next_node->function_id = f - functions;
  next_node->entry_time = time;
  next_node->exit_time = 0;

  next_node->caller = caller;

  next_node->parent = call_tree_current_node;
  next_node->children = NULL;
  next_node->nchildren = 0;

  call_tree_current_node = next_node;

  return 0;
}

int
function_exit (uintptr_t address __attribute__ ((unused)), unsigned long long time)
{
  //function *f = function_get_by_address(address);

  //if (!f)
  //  {
  //    feedback_warning("0x%" PRIxPTR ": exting unknown function - skewed trace data?");
  //    return 0;
  //  }

  if (call_tree_current_node->function_id == (unsigned int)-1 /*|| functions[call_tree_current_node->function_id].address != address*/)
    {
      feedback_warning("0x%" PRIxPTR ": exiting function not on top of the call stack - skewed trace data?");
      return 0;
    }

  call_tree_current_node->exit_time = time;
  call_tree_current_node = call_tree_current_node->parent;

  return 0;
}

int 
function_exit_all (unsigned long long time)
{
  while (call_tree_current_node->function_id != (unsigned int)-1)
    {
      uintptr_t func = functions[call_tree_current_node->function_id].address;
      int res = function_exit(func, time);
      assert_inner(!res, "function_exit");
    }

  function_aggregate_recursive(&call_tree_root);

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

function*
function_get_all(unsigned int *nfunctions_ptr)
{
  *nfunctions_ptr = nfunctions;
  return functions;
}
