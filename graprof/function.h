
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013 - 2014  Andreas Grapentin                            *
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


#pragma once

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdint.h>

#include <grapes/util.h>

/* this struct represents en entry in the call tree of the profiled process.
 */

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

/* this struct holds the call data information of one side of a caller-callee
 * pair, as well as some aggregated timing values.
 */
struct call_data
{
  unsigned int function_id;
  unsigned long calls;
  unsigned long long self_time;
  unsigned long long children_time;
};

typedef struct call_data call_data;

/* this struct holds all information related to a function
 */
struct function
{
  uintptr_t address;
  char *name;
  char *file;
  unsigned int line;

  unsigned long long self_time;
  unsigned long long children_time;
  unsigned long calls;

  call_data *callers;
  unsigned int ncallers;
  call_data *callees;
  unsigned int ncallees;

  tree_entry **primaries;
  unsigned int nprimaries;
  tree_entry **secondaries;
  unsigned int nsecondaries;
};

typedef struct function function;

/* process an enter event for a given function, that has been read from the
 * trace data
 *
 * params:
 *   address - the address of the function entry
 *   caller - thr return address of the function minus 4
 *   time - the time of the call in ns
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc and realloc
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int function_enter(uintptr_t address, uintptr_t caller, unsigned long long time) MAY_FAIL;

/* process an exit event for the last entered function, that has been read
 * from the trace data
 *
 * params:
 *   time - the time of the return in ns
 */
void function_exit(unsigned long long time);

/* this function is called at the end of the trace data, representing child
 * termination. The call stack is cleared by exiting all unexited functions,
 * and the time aggregation is initiated.
 *
 * params:
 *   time - the time of the end symbol in ns
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc and realloc
 *
 * returns:
 *   -1 on failure, 0 on success
 */
int function_exit_all(unsigned long long time) MAY_FAIL;

/* get the function represented by a given address
 *
 * params:
 *   address - the address of the requested function
 *
 * returns:
 *   a pointer to the function, if avaiable, NULL otherwise
 */
function* function_get_by_address(uintptr_t address);

/* get the list of known functions
 *
 * params:
 *   nfunctions - a pointer where the length of the returned array is stored
 *
 * returns:
 *   a pointer to the first of an array of functions
 */
function* function_get_all(unsigned int *nfunctions);

/* get the function currently on the top of the call stack
 *
 * returns:
 *   a pointer to the function currently on top of the call stack, if not
 *   empty, NULL otherwise
 */
function* function_get_current(void);

/* compare a function and an address, translate the address to file name and
 * function name, and return the result of the string comparison of both,
 * analogous to strcmp.
 *
 * errors:
 *   may (or may not) fail and set errno for the same reasons as addr_translate
 *
 * returns:
 *   -1 on failure,
 *   < 0, if function "<" address
 *   0, if function "=" address
 *   > 0, if function ">" address
 */
int function_compare(function *f, uintptr_t addr) MAY_FAIL;

/* get the total number of function calls
 *
 * returns:
 *   the total number of functions called during profilee runtime
 */
unsigned long long function_get_total_calls(void);

/* get the number of distinct function in the call tree
 *
 * returns:
 *   the total number of distinct functions
 */
unsigned int function_get_nfunctions(void);

