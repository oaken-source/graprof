
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

/* This is the user feedback module. It contains output control mechanisms and 
 * a progress meter as well as some convenience error messaging macros.
 */

#include <stdlib.h>
#include "util.h"

#define feedback_assert(COND, ...) do { if(!(COND)) feedback_error(EXIT_FAILURE, __VA_ARGS__); } while (0)
#define feedback_assert_wrn(COND, ...) do { if(!(COND)) feedback_warning(__VA_ARGS__); } while (0)

/* prints a formatted error string to stderr, prepends the program name and if
 * errno != 0 appends the appropriate string representation. terminates the 
 * program if status != EXIT_SUCCESS
 *
 * params:
 *   status - the parameter to exit() if != EXIT_SUCCESS
 *   format, ... - the formatted error string
 */
void feedback_error(int status, const char *format, ...);

/* prints a formatted error string to stderr similar to the one written by the
 * GNU extension fuction error_at_line, and is mainly used by the assertion 
 * macros defined in util.h

 * params:
 *   filename - the name of the file where the error lies (usually __FILE__)
 *   linenum - the line index of the error (usually __LINE__)
 *   format, ... - the formatted error string
 */
void feedback_error_at_line(const char *filename, unsigned int linenum, const char *format, ...);

/* prints a formatted warning string to stderr, prepends the program name and
 * if errno != 0 appends the approproate string representation.
 *
 * params:
 *   format, ... - the formatted warning string
 */
void feedback_warning(const char *format, ...);

/* finishes the currently running progress meter, if any, and starts a new one
 * with the given tag. 
 *
 * params:
 *   tag - the tag of the progress meter
 *   max - the number of increments that equal 100%. if this is set to zero,
 *         the progress bar will just be a spinner.
 */
void feedback_progress_start(const char *tag, unsigned long max);

/* increments the currently running progress meter, if any, and outputs any
 * changes of the progress percentile to stdout
 */
void feedback_progress_inc();

/* finishes the currently running progress meter, if any.
*/
void feedback_progress_finish();

/* prompt for a value and return the string input by the user
 *
 * params:
 *   prompt - the prompt shown to the user. if prompt is NULL, no prompt is 
 *            shown
 *
 * returns:
 *   NULL, if there was an error, the string input by the user otherwise.
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc and realloc
 */
char *feedback_readline(const char *prompt);

