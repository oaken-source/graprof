
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


#pragma once

#include <stdint.h>

struct call_data
{
  unsigned int function_id;
  unsigned long calls;
  unsigned long long self_time;
  unsigned long long children_time;
};

typedef struct call_data call_data;

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
};

typedef struct function function;

int function_enter(uintptr_t, uintptr_t, unsigned long long);

int function_exit(unsigned long long);

int function_exit_all(unsigned long long);

function* function_get_by_address(uintptr_t);

function* function_get_all(unsigned int*);

function* function_get_current();

int function_compare(function *f, uintptr_t addr);
