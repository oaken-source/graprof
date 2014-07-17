
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


#include "traceview_scrollbar.h"

#if HAVE_NCURSES

WINDOW *traceview_scrollbar = NULL;

int
traceview_scrollbar_init (void)
{
  traceview_scrollbar = newwin(1, 1, 1, 1);
  assert_inner(traceview_scrollbar, "newwin");

  return 0;
}

int
traceview_scrollbar_redraw (int y, int x, int height, float state)
{
  int res = wresize(traceview_scrollbar, height, 1);
  assert_inner(res != ERR, "wresize");

  res = mvwin(traceview_scrollbar, y, x);
  assert_inner(res != ERR, "mvwin");

  res = werase(traceview_scrollbar);
  assert_inner(res != ERR, "werase");

  res = mvwvline(traceview_scrollbar, 0, 0, 0, height);
  assert_inner(res != ERR, "mvwvline");

  mvwaddch(traceview_scrollbar, 0, 0, ACS_UARROW);
  mvwaddch(traceview_scrollbar, height - 1, 0, ACS_DARROW);
  mvwaddch(traceview_scrollbar, 1 + state * (height - 2), 0, ACS_BLOCK);

  res = wrefresh(traceview_scrollbar);
  assert_inner(res != ERR, "wrefresh");

  return 0;
}

#endif // HAVE_NCURSES
