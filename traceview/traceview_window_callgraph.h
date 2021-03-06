
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2014  Andreas Grapentin                                   *
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

#if HAVE_NCURSES

#include <grapes/util.h>

#include "traceview_keys.h"

#include <ncurses.h>

extern WINDOW *traceview_window_callgraph;

int traceview_window_callgraph_init(void) __may_fail;

int traceview_window_callgraph_redraw(void) __may_fail;

int traceview_window_callgraph_key_dispatch(traceview_key k) __may_fail;

int traceview_window_callgraph_navigate_to_function(unsigned int function_id) __may_fail;

#endif // HAVE_NCURSES
