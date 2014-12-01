
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
 *   COND - the condition to check
 */
#define static_assert(COND) typedef char _static_assertion[(!!(COND))*2-1]

/* runtime assertion
 * evaluate conditions at runtime and handle them differently
 *
 * these macros have different flavours and should be used in different
 * situations, where errors that occur are handed back to through the call
 * stack. Each of these functions has printf-like behaviour, if DEBUG has been
 * defined by the calling application, printing a formatted error message to
 * stderr, with varying additional behaviour as described below:
 *
 * assert_inner:
 *   return -1
 * assert_inner_ptr:
 *   return NULL
 * assert_set_errno:
 *   set errno to the given errnum and return -1
 * assert_set_errno_ptr:
 *   set errno to the given errnum and return NULL
 * assert_weak:
 *   only print the error string, but not return
 */
#define _assert_feedback(...) \
    do { if (DEBUG) { feedback_error_at_line(__FILE__, __LINE__, __VA_ARGS__); } } while (0)

#define assert_inner(COND, ...) \
    do { if (__unlikely(!(COND))) { _assert_feedback(__VA_ARGS__); return -1; } } while(0)
#define assert_inner_ptr(COND, ...) \
    do { if (__unlikely(!(COND))) { _assert_feedback(__VA_ARGS__); return NULL; } } while(0)
#define assert_set_errno(COND, ERRNUM, ...) \
    do { if (__unlikely(!(COND))) { errno = ERRNUM; _assert_feedback(__VA_ARGS__); return -1; } } while(0)
#define assert_set_errno_ptr(COND, ERRNUM, ...) \
    do { if (__unlikely(!(COND))) { errno = ERRNUM; _assert_feedback(__VA_ARGS__); return NULL; } } while(0)
#define assert_weak(COND, ...) \
    do { if (__unlikely(!(COND))) { _assert_feedback(__VA_ARGS__); } } while (0)

/* runtime assertion using the macros above
 */
#define __checked_call(RES, FUNC) \
    do { assert_inner((RES) == (FUNC), # FUNC); } while (0)
#define __checked_call_ptr(RES, FUNC) \
    do { assert_inner_ptr((RES) == (FUNC), # FUNC); } while (0)

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
