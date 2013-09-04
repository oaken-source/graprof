
/******************************************************************************
 *                                  Grapes                                    *
 *                                                                            *
 *    Copyright (C) 2013 Andreas Grapentin                                    *
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
#include <error.h>
#include <stdlib.h>

/* This unit provides a set of miscellaneous macros and enumerations used 
 * throughout the project
 */

#ifndef DEBUG
#  define DEBUG 0
#endif

/* convenience macro - 
 * if the given condition is false, call errro_at_line(...) if in debug mode, 
 * return -1 otherwise. this function is designed to catch and handle errors 
 * to library calls from library calls to pass errors up to the main program
 */
#if DEBUG && defined(__GNUC__)
#  define assert_inner(COND, ...) if(!(COND)) { error_at_line(0, errno, __FILE__, __LINE__, __VA_ARGS__); return -1; }
#  define assert_inner_ptr(COND, ...) if(!(COND)) { error_at_line(0, errno, __FILE__, __LINE__, __VA_ARGS__); return NULL; }
#else
#  define assert_inner(COND, ...) if(!(COND)) return -1
#  define assert_inner_ptr(COND, ...) if(!(COND)) return NULL
#endif

/* convenience macro - 
 * if the given condition is false, set errno to the given errnum and return -1
 */
#if DEBUG && defined(__GNUC__)
#  define assert_set_errno(COND, ERRNUM, ...) if(!(COND)) { errno = ERRNUM; error_at_line(EXIT_SUCCESS, errno, __FILE__, __LINE__, __VA_ARGS__); return -1; }
#  define assert_set_errno_ptr(COND, ERRNUM, ...) if(!(COND)) { errno = ERRNUM; error_at_line(EXIT_SUCCESS, errno, __FILE__, __LINE__, __VA_ARGS__); return NULL; }
#else
#  define assert_set_errno(COND, ERRNUM, ...) if(!(COND)) { errno = ERRNUM; return -1; }
#  define assert_set_errno_ptr(COND, ERRNUM, ...) if(!(COND)) { errno = ERRNUM; return NULL; }
#endif

/* color escape codes */
#define YELLOW	"\033[1;33m"
#define GREEN	"\033[1;32m"
#define RED	"\033[1;31m"
#define NORMAL	"\033[0m"

/* use warn_unused_result if avilable */
#ifdef __GNUC__
#  define WR __attribute__((warn_unused_result))
#else
#  define WR
#endif
