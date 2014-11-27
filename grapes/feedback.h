
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

/* This is the user feedback module. It contains output control mechanisms and
 * some convenience error messaging macros.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <grapes/util.h>

#include <stdlib.h>


/* convenience assertion macro - if the given condition is false, print a
 * formatted error string to stderr using feedback_error, and terminate the
 * program
 *
 * params:
 *   COND - the condition to test
 *   ... - the parameters passed to feedback_error
 */
#define feedback_assert(COND, ...) \
    do { if(!(COND)) feedback_error(EXIT_FAILURE, __VA_ARGS__); } while (0)

/* convenience assertion macro - if the given condition is false, print a
 * formatted warning string to stderr using feedback_warning
 *
 * params:
 *   COND - the condition to test
 *   ... - the parameters passed to feedback_warning
 */
#define feedback_assert_wrn(COND, ...) \
    do { if(!(COND)) feedback_warning(__VA_ARGS__); } while (0)

/* prints a formatted error string to stderr similar to the one written by the
 * GNU extension fuction error_at_line, and is mainly used by the assertion
 * macros defined in util.h
 *
 * params:
 *   filename - the name of the file where the error lies (usually __FILE__)
 *   linenum - the line index of the error (usually __LINE__)
 *   format, ... - the formatted error string
 */
void feedback_error_at_line(const char *filename, unsigned int linenum, const char *format, ...);

/* prints a formatted error string to stderr, prepends the string contained
 * in the variable program_invocation_short_name provided by glibc, if
 * available, and if errno != 0 appends the appropriate string representation.
 * terminates the program if status != EXIT_SUCCESS
 *
 * params:
 *   status - the parameter to exit() if != EXIT_SUCCESS
 *   format, ... - the formatted error string
 */
void feedback_error(int status, const char *format, ...);

/* prints a formatted warning string to stderr, prepends the string contained
 * in the variable program_invocation_short_name provided by glibc, if
 * available, and if errno != 0 appends the appropriate string representation.
 *
 * params:
 *   format, ... - the formatted warning string
 */
void feedback_warning(const char *format, ...);

