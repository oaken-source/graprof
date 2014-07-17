
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

WINDOW *traceview_help_overlay = NULL;

static unsigned int traceview_help_overlay_width = 34;
static unsigned int traceview_help_overlay_height = 8;

int
traceview_help_overlay_init (void)
{
  traceview_help_overlay = newwin(1, 1, 1, 1);
  assert_inner(traceview_help_overlay, "newwin");

  int res = traceview_help_overlay_redraw();
  assert_inner(!res, "traceview_help_overlay_redraw");

  return 0;
}

int
traceview_help_overlay_redraw (void)
{
  int res = wresize(traceview_help_overlay, traceview_help_overlay_height, traceview_help_overlay_width);
  assert_inner(res != ERR, "wresize");

  unsigned int x = COLS  / 2 - traceview_help_overlay_width  / 2;
  unsigned int y = LINES / 2 - traceview_help_overlay_height / 2;

  res = mvwin(traceview_help_overlay, y, x);
  assert_inner(res != ERR, "mvwin");

  res = werase(traceview_help_overlay);
  assert_inner(res != ERR, "werase");

  res = box(traceview_help_overlay, 0, 0);
  assert_inner(res != ERR, "wbkgd");

  mvwprintw(traceview_help_overlay, 0, 2, " graprof traceview help ");
  mvwprintw(traceview_help_overlay, 2, 2, "F1  - show / dismiss this help");
  mvwprintw(traceview_help_overlay, 3, 2, "ESC - back");
  mvwprintw(traceview_help_overlay, 4, 2, "q   - back / exit");
  mvwprintw(traceview_help_overlay, 6, 2, "ALT + [1-4] - switch view");

  res = wrefresh(traceview_help_overlay);
  assert_inner(res != ERR, "wrefresh");

  return 0;
}

#endif // HAVE_NCURSES
