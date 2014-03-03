
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

#include <errno.h>
#include <stdlib.h>

/* sanitize common macro definitions */
#ifndef DEBUG
#  define DEBUG 0
#endif

/* this function is used by the assertion macros below, to pretty print debug messages */
void feedback_error_at_line(const char *filename, unsigned int linenum, const char *format, ...);

/* assertion macros - define hell incoming!
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
 */
#if DEBUG
#  define assert_inner(COND, ...) do { if(!(COND)) { feedback_error_at_line(__FILE__, __LINE__, __VA_ARGS__); return -1; } } while (0)
#  define assert_inner_ptr(COND, ...) do { if(!(COND)) { feedback_error_at_line(__FILE__, __LINE__, __VA_ARGS__); return NULL; } } while (0)
#  define assert_set_errno(COND, ERRNUM, ...) do { if(!(COND)) { errno = ERRNUM; feedback_error_at_line(__FILE__, __LINE__, __VA_ARGS__); return -1; } } while (0)
#  define assert_set_errno_ptr(COND, ERRNUM, ...) do { if(!(COND)) { errno = ERRNUM; feedback_error_at_line(__FILE__, __LINE__, __VA_ARGS__); return NULL; } } while (0) 
#else
#  define assert_inner(COND, ...) do { if(!(COND)) return -1; } while (0)
#  define assert_inner_ptr(COND, ...) do { if(!(COND)) return NULL; } while (0)
#  define assert_set_errno(COND, ERRNUM, ...) do { if(!(COND)) { errno = ERRNUM; return -1; } } while (0)
#  define assert_set_errno_ptr(COND, ERRNUM, ...) do { if(!(COND)) { errno = ERRNUM; return NULL; } } while (0)
#endif

/* convenience shortcuts for attributes */
#ifdef __GNUC__
#  define __WR__ __attribute__((warn_unused_result))
#  define __UU__ __attribute__((unused))
#else
#  define __WR__
#  define __UU__
#endif

/* and with some semantic sugar */
#ifdef __GNUC__
#  define MAY_FAIL __attribute__((warn_unused_result))
#else
#  define MAY_FAIL 
#endif

/* subset of available ANSI color escape codes */
#define YELLOW	"\033[1;33m"
#define GREEN	"\033[1;32m"
#define RED	"\033[1;31m"
#define NORMAL	"\033[0m"

