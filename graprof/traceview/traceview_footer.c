
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


#include "traceview_footer.h"

#if HAVE_NCURSES

#include <ncurses.h>

static WINDOW *traceview_footer = NULL;

static unsigned int traceview_footer_index = 1;

int
traceview_footer_init (void)
{
  traceview_footer = newwin(1, COLS, LINES - 1, 0);
  assert_inner(traceview_footer, "newwin");

  int res = traceview_footer_redraw();
  assert_inner(!res, "traceview_footer_redraw");

  return 0;
}

int
traceview_footer_redraw (void)
{
  int res = wresize(traceview_footer, 1, COLS);
  assert_inner(res != ERR, "wresize");

  res = mvwin(traceview_footer, LINES - 1, 0);
  assert_inner(res != ERR, "mvwin");

  res = werase(traceview_footer);
  assert_inner(res != ERR, "werase");

  res = wbkgd(traceview_footer, COLOR_PAIR(1));
  assert_inner(res != ERR, "wbkgd");

  res = traceview_footer_set_index(traceview_footer_index);
  assert_inner(!res, "traceview_footer_set_index");

  return 0;
}

int
traceview_footer_set_index (unsigned int index)
{
  assert_set_errno(index >= 1 && index <= 4, EINVAL, "index out of range");

  traceview_footer_index = index;

  int res = wattron(traceview_footer, COLOR_PAIR(1));
  assert_inner(res != ERR, "wattron");

  mvwprintw(traceview_footer, 0, COLS - 16, "[1] [2] [3] [4]");

  res = wattron(traceview_footer, A_STANDOUT);
  assert_inner(res != ERR, "wattron");

  mvwaddch(traceview_footer, 0, COLS - 16 + 1 + (index - 1) * 4, index + '0');

  res = wrefresh(traceview_footer);
  assert_inner(res != ERR, "wrefresh");

  res = wattroff(traceview_footer, A_STANDOUT | COLOR_PAIR(1));
  assert_inner(res != ERR, "wattroff");

  return 0;
}

#endif // HAVE_NCURSES
