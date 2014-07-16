
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


#include "traceview_help_overlay.h"

#if HAVE_NCURSES

#include <ncurses.h>

WINDOW *traceview_help_overlay = NULL;

int
traceview_help_overlay_init (void)
{
  traceview_help_overlay = newwin(LINES - 16, COLS - 14, 8, 7);
  assert_inner(traceview_help_overlay, "newwin");

  int res = traceview_help_overlay_redraw();
  assert_inner(!res, "traceview_help_overlay_redraw");

  return 0;
}

int
traceview_help_overlay_redraw (void)
{
  int res = wresize(traceview_help_overlay, LINES - 16, COLS - 14);
  assert_inner(res != ERR, "wresize");

  res = mvwin(traceview_help_overlay, 8, 7);
  assert_inner(res != ERR, "mvwin");

  res = werase(traceview_help_overlay);
  assert_inner(res != ERR, "werase");

  res = box(traceview_help_overlay, 0, 0);
  assert_inner(res != ERR, "wbkgd");

  mvwprintw(traceview_help_overlay, 0, 4, " graprof traceview help ");

  res = wrefresh(traceview_help_overlay);
  assert_inner(res != ERR, "wrefresh");

  return 0;
}

#endif // HAVE_NCURSES
