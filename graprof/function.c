
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


#include "function.h"
#include "addr.h"

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <grapes/util.h>
#include <grapes/feedback.h>

static function *functions = NULL;
static unsigned int nfunctions = 0;

unsigned long long total_calls = 0;

struct tree_entry
{
  unsigned int function_id;
  unsigned long long entry_time;
  unsigned long long exit_time;

  unsigned long long self_time;
  unsigned long long children_time;
  unsigned long long orphan_time;

  struct tree_entry *parent;
  struct tree_entry **children;
  unsigned int nchildren;
  struct tree_entry **orphans;
  unsigned int norphans;
};

typedef struct tree_entry tree_entry;

static tree_entry call_tree_root = { -1, 0, 0, 0, 0, 0, NULL, NULL, 0, NULL, 0 };
static tree_entry *call_tree_current_node = &call_tree_root;

static void
function_init (function *f)
{
  f->address = 0;
  f->name = NULL;
  f->file = NULL;
  f->line = 0;
  f->self_time = 0;
  f->children_time = 0;
  f->calls = 0;
  f->callers = NULL;
  f->ncallers = 0;
  f->callees = NULL;
  f->ncallees = 0;
}

static void
function_call_tree_entry_init (tree_entry *e)
{
  e->function_id = -1;
  e->entry_time = 0;
  e->exit_time = 0;
  e->self_time = 0;
  e->children_time = 0;
  e->orphan_time = 0;
  e->parent = NULL;
  e->children = NULL;
  e->nchildren = 0;
  e->orphans = NULL;
  e->norphans = 0;
}

static void
function_call_data_init (call_data *d)
{
  d->function_id = 0;
  d->calls = 0;
  d->self_time = 0;
  d->children_time = 0;
}

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

static int 
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
}

static void
function_aggregate_call_tree_node_times (tree_entry *t)
{
  unsigned int i;
  for (i = 0; i < t->nchildren; ++i)
    {
      function_aggregate_call_tree_node_times(t->children[i]);
      t->children_time += t->children[i]->self_time + t->children[i]->children_time;
      t->orphan_time += t->children[i]->orphan_time;
    }

  for (i = 0; i < t->norphans; ++i)
    {
      function_aggregate_call_tree_node_times(t->orphans[i]);
      t->orphan_time += t->orphans[i]->self_time + t->orphans[i]->children_time + t->orphans[i]->orphan_time;
    }

  t->self_time = t->exit_time - t->entry_time - t->children_time - t->orphan_time;
}

static void
function_add_caller_self_time (unsigned int caller_id, unsigned int callee_id, unsigned long long time)
{
  function *f = functions + caller_id;
  
  unsigned int i;
  for (i = 0; i < f->ncallees; ++i)
    if (f->callees[i].function_id == callee_id)
      f->callees[i].self_time += time;

  f = functions + callee_id;

  for (i = 0; i < f->ncallers; ++i)
    if (f->callers[i].function_id == caller_id)
      f->callers[i].self_time += time;
}

static void
function_add_caller_children_time (unsigned int caller_id, unsigned int callee_id, unsigned long long time)
{
  function *f = functions + caller_id;
  
  unsigned int i;
  for (i = 0; i < f->ncallees; ++i)
    if (f->callees[i].function_id == callee_id)
      f->callees[i].children_time += time;

  f = functions + callee_id;

  for (i = 0; i < f->ncallers; ++i)
    if (f->callers[i].function_id == caller_id)
      f->callers[i].children_time += time;
}


static void
function_aggregate_function_time_for_id (tree_entry *e, unsigned int function_id, unsigned int found)
{
  if (e->function_id == function_id)
    {
      functions[function_id].self_time += e->self_time;
      if (e->parent != NULL && e->parent->function_id != (unsigned int)-1)
          function_add_caller_self_time(e->parent->function_id, e->function_id, e->self_time);
      if (!found)
        {
          functions[function_id].children_time += e->children_time;
          if (e->parent != NULL && e->parent->function_id != (unsigned int)-1)
            function_add_caller_children_time(e->parent->function_id, e->function_id, e->children_time);
        }
      found = 1;
    }

  unsigned int i;
  for (i = 0; i < e->nchildren; ++i)
    function_aggregate_function_time_for_id (e->children[i], function_id, found);
  for (i = 0; i < e->norphans; ++i)
    function_aggregate_function_time_for_id (e->orphans[i], function_id, found);
}

static void
function_aggregate_function_times ()
{
  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
    function_aggregate_function_time_for_id(&call_tree_root, i, 0);
}

int
function_enter (uintptr_t address, uintptr_t caller, unsigned long long time)
{
  ++total_calls;

  function *f = function_get_by_address(address);
  if (!f)
    f = function_push(address);
  assert_inner(f, "function_push");

  ++(f->calls);

  tree_entry *n = call_tree_current_node;
  tree_entry *next = malloc(sizeof(*next));
  assert_inner(next,  "malloc");

  unsigned int caller_id = (unsigned int)-1;

  if (n->parent != NULL && !function_compare(functions + n->function_id, caller))
    {
      caller_id = n->function_id;

      if (n->function_id != (unsigned int)-1)
        {
          int res = function_add_callee(functions + n->function_id, f - functions);
          assert_inner(!res, "function_add_callee");
        }

      ++(n->nchildren);
      n->children = realloc(n->children, sizeof(*(n->children)) * n->nchildren);
      assert_inner(n->children, "realloc");

      n->children[n->nchildren - 1] = next;
    }
  else
    {
      ++(n->norphans);
      n->orphans = realloc(n->orphans, sizeof(*(n->orphans)) * n->norphans);
      assert_inner(n->orphans, "realloc");

      n->orphans[n->norphans - 1] = next;
    }

  int res = function_add_caller(f, caller_id);
  assert_inner(!res, "function_add_callee");

  function_call_tree_entry_init(next);
  next->function_id = f - functions;
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

  function_aggregate_call_tree_node_times(&call_tree_root);
  function_aggregate_function_times();

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
function_get_all (unsigned int *n)
{
  *n = nfunctions;
  return functions;
}

function*
function_get_current ()
{
  if (call_tree_current_node->function_id != (unsigned int)-1)
    return functions + call_tree_current_node->function_id;
  return NULL;
}

int
function_compare (function *f, uintptr_t addr)
{
  char *name;
  char *file;
  int res = addr_translate(addr, &name, &file, NULL);
  assert_inner(!res, "addr_translate");

  res = (strcmp(f->name, name) || strcmp(f->file, file));

  free(name);
  free(file);

  return res;
}

unsigned long long
function_get_total_calls ()
{
  return total_calls;
}

#if FREE_ALL_MEMORY_EXPLICITLY

static void
free_call_tree (tree_entry *e)
{
  unsigned int i;
  for (i = 0; i < e->nchildren; ++i)
    {
      free_call_tree(e->children[i]);
      free(e->children[i]);
    }
  for (i = 0; i < e->norphans; ++i)
    {
      free_call_tree(e->orphans[i]);
      free(e->orphans[i]);
    }

  free(e->children);
  free(e->orphans);
}

static void
__attribute__((destructor))
function_fini ()
{
  unsigned int i;
  for (i = 0; i < nfunctions; ++i)
    {
      free(functions[i].name);
      free(functions[i].file);
      free(functions[i].callers);
      free(functions[i].callees);
    }
  free(functions);

  free_call_tree(&call_tree_root);
}

#endif // FREE_ALL_MEMORY_EXPLICITLY
