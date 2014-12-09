
/******************************************************************************
 *                                  Grapes                                    *
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
# include <config.h>
#endif

#include <errno.h>
#include <stdlib.h>
#include <stddef.h>

/* sanitize common macro definitions */
#ifndef DEBUG
#  define DEBUG 0
#endif

/* this function is used by the assertion macros below, to pretty print debug messages */
void feedback_error_at_line(const char *filename, unsigned int linenum, const char *format, ...);

/* static assertion
 * evaluate conditions at compile time and fail if not met
 *
 * params:
 *   C - the condition to check
 */
#define static_assert(C) typedef char _static_assertion[(!!(C))*2-1]

/* miscellaneous helper macros, not to be used directly
 */
static const typeof(-1) _assert_return = -1;
#define __returns_ptr typeof(NULL) _assert_return = NULL
#define _assert_generic(C, E, F, ...) \
    do { \
      if (__likely(C))  break; \
      if (E)            errno = (E); \
      if (DEBUG)        feedback_error_at_line(__FILE__, __LINE__, __VA_ARGS__); \
      F \
      return _assert_return; \
    } while (0)

#define __checked_call_1(C)         _assert_generic((C),   0,  , "checked call failed: %s", # C)
#define __checked_call_2(C, F)      _assert_generic((C),   0, F, "checked call failed: %s", # C)

#define __precondition_1(C)         _assert_generic((C),   0,  , "precondition failed: %s", # C)
#define __precondition_2(E, C)      _assert_generic((C), (E),  , "precondition failed: %s", # C)

#define __get_third(arg1, arg2, arg3, ...) arg3

#define __checked_call_disp(...)    __get_third(__VA_ARGS__, __checked_call_2, __checked_call_1)
#define __precondition_disp(...)    __get_third(__VA_ARGS__, __precondition_2, __precondition_1)

/* this macro provides runtime assertions for nested call checking.
 * If the given condition is false, a string representation is printed to
 * stderr and a value determined by the __returns_* macros is returned.
 * If a second parameter is specified, then it is evaluated as a statement
 * before returning.
 *
 * params:
 *   C - an expression containing a nested call that evaluates to an integer,
 *      for example `0 == some_function()`
 *   FUNC [optional] - a statement to be executed on failure
 */
#define __checked_call(...)         __checked_call_disp(__VA_ARGS__)(__VA_ARGS__)

/* this macro is similar to __checked_call, but has a different semantic. It
 * is intended to allow for function input checking, and optionally setting
 * errno on failure.
 *
 * params:
 *   ERRNUM [default: 0] - the new value of errno, if != 0 and the condition
 *      is false
 *   C - an expression that evaluates to an integer used as the checked value
 */
#define __precondition(...)         __precondition_disp(__VA_ARGS__)(__VA_ARGS__)

/* convenience min/max macros
 *
 * note that these macros evaluate their parameters more than once.
 */
#define max(X, Y) ((X) > (Y) ? (X) : (Y))
#define min(X, Y) ((X) > (Y) ? (Y) : (X))

/* convenience attribute shortcuts with semantic sugar */
#ifdef __GNUC__
#  define __may_fail    __attribute__((warn_unused_result))
#  define __unused      __attribute__((unused))
#  define __debug_func  __attribute__((deprecated))
#else
#  define __may_fail
#  define __unused
#  define __debug_func
#endif

/* branch optimization macros */
#ifdef __GNUC__
#  define __likely(X)   __builtin_expect((X),1)
#  define __unlikely(X) __builtin_expect((X),0)
#else
#  define __likely(X)   (X)
#  define __unlikely(X) (X)
#endif
