
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


#include "traceview_window_interactive.h"

#if HAVE_NCURSES

#include <ncurses.h>

WINDOW *traceview_window_interactive = NULL;

int
traceview_window_interactive_init (void)
{
  traceview_window_interactive = newwin(LINES - 2, COLS, 1, 0);
  assert_inner(traceview_window_interactive, "newwin");

  return 0;
}

int
traceview_window_interactive_redraw (void)
{
  int res = wresize(traceview_window_interactive, LINES - 2, COLS);
  assert_inner(res != ERR, "wresize");

  res = mvwin(traceview_window_interactive, 1, 0);
  assert_inner(res != ERR, "mvwin");

  res = werase(traceview_window_interactive);
  assert_inner(res != ERR, "werase");

  return 0;
}

int
traceview_window_interactive_key_dispatch (unused char c)
{
  return 0;
}


#endif // HAVE_NCURSES
