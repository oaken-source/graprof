
#include "function.h"
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

  unsigned long long self_time;
  unsigned long long cumulative_time;

  struct tree_entry *parent;
  struct tree_entry *children;
  unsigned int nchildren;
  struct tree_entry *orphans;
  unsigned int norphans;
};

typedef struct tree_entry tree_entry;

static tree_entry call_tree_root = { -1, 0, 0, 0, 0, NULL, NULL, 0, NULL, 0 };
static tree_entry *call_tree_current_node = &call_tree_root;

static void
function_init (function *f)
{
  f->address = 0;
  f->name = NULL;
  f->file = NULL;
  f->line = 0;
  f->self_time = 0;
  f->cumulative_time = 0;
  f->calls = 0;
  f->callers = 0;
  f->ncallers = 0;
  f->callees = 0;
  f->ncallees = 0;
}

static void
function_call_tree_entry_init (tree_entry *e)
{
  e->function_id = -1;
  e->entry_time = 0;
  e->exit_time = 0;
  e->self_time = 0;
  e->cumulative_time = 0;
  e->parent = NULL;
  e->children = NULL;
  e->nchildren = 0;
  e->orphans = NULL;
  e->norphans = 0;
}

/*static void
function_call_data_init (call_data *d)
{
  d->function_id = 0;
  d->calls = 0;
  d->self_time = 0;
  d->children_time = 0;
}*/

static function* 
function_push (uintptr_t address)
{
  ++nfunctions;
  functions = realloc(functions, sizeof(*functions) * nfunctions);
  assert_inner_ptr(functions, "realloc");

  function *f = functions + nfunctions - 1;
  function_init(f);

  f->address = address;
  addr_translate(f->address, &(f->name), &(f->file), &(f->line));

  feedback_assert_wrn(strcmp(f->name, "??"), "unable to identify instrumented function at 0x%" PRIxPTR " - missing debug symbols?", address);

  return f;
}

/*static int 
function_add_caller (function *f, unsigned int caller_id)
{
  unsigned int i;
  for (i = 0; i < f->ncallers; ++i)
    if (f->callers[i].function_id == caller_id)
      {
        ++(f->callers[i].calls);
        return 0;
      }

  ++(f->ncallers);
  f->callers = realloc(f->callers, sizeof(*(f->callers)) * f->ncallers);
  assert_inner(f->callers, "realloc");

  call_data *d = f->callers + f->ncallers - 1;
  function_call_data_init(d);
  d->function_id = caller_id;
  d->calls = 1;

  return 0;
}

static int
function_add_callee (function *f, unsigned int callee_id)
{
  unsigned int i;
  for (i = 0; i < f->ncallees; ++i)
    if (f->callees[i].function_id == callee_id)
      {
        ++(f->callees[i].calls);
        return 0;
      }

  ++(f->ncallees);
  f->callees = realloc(f->callees, sizeof(*(f->callees)) * f->ncallees);
  assert_inner(f->callees, "realloc");

  call_data *d = f->callees + f->ncallees - 1;
  function_call_data_init(d);
  d->function_id = callee_id;
  d->calls = 1;

  return 0;
}*/

static int
function_compare (function *f, uintptr_t addr)
{
  char *name;
  char *file;
  int res = addr_translate(addr, &name, &file, NULL);
  assert_inner(!res, "addr_translate");

  return (strcmp(f->name, name) || strcmp(f->file, file));
}

static void
function_call_tree_aggregate_node_times (tree_entry *t)
{
  t = t;
}

int
function_enter (uintptr_t address, uintptr_t caller, unsigned long long time)
{
  function *f = function_get_by_address(address);
  if (!f)
    f = function_push(address);
  assert_inner(f, "function_push");

  tree_entry *n = call_tree_current_node;
  tree_entry *next = NULL;

  if (n->parent == NULL || !function_compare(functions + n->function_id, caller))
    {
      ++(n->nchildren);
      n->children = realloc(n->children, sizeof(*(n->children)) * n->nchildren);
      assert_inner(n->children, "realloc");

      next = n->children + n->nchildren - 1;
    }
  else
    {
      ++(n->norphans);
      n->orphans = realloc(n->orphans, sizeof(*(n->orphans)) * n->norphans);
      assert_inner(n->orphans, "realloc");

      next = n->orphans + n->norphans - 1;
    }

  function_call_tree_entry_init(next);
  next->function_id = address;
  next->entry_time = time;
  next->parent = call_tree_current_node;

  call_tree_current_node = next;

  return 0;
}

int
function_exit (unsigned long long time)
{
  if (call_tree_current_node->parent == NULL)
    {
      feedback_warning("exit called on empty call stack - skewed trace data?");
      return 0;
    }

  call_tree_current_node->exit_time = time;
  call_tree_current_node = call_tree_current_node->parent;

  return 0;
}

int 
function_exit_all (unsigned long long time)
{
  while (call_tree_current_node->parent != NULL)
    {
      int res = function_exit(time);
      assert_inner(!res, "function_exit");
    }

  function_call_tree_aggregate_node_times(&call_tree_root);

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
