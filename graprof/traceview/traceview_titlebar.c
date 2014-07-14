
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


#include "traceview_titlebar.h"

#if HAVE_NCURSES

#include <ncurses.h>

static WINDOW *traceview_titlebar = NULL;

int
traceview_titlebar_init (void)
{
  traceview_titlebar = newwin(1, COLS, 0, 0);
  assert_inner(traceview_titlebar, "newwin");

  int res = wbkgd(traceview_titlebar, COLOR_PAIR(1));
  assert_inner(res != ERR, "wbkgd");

  res = traceview_titlebar_set_title(NULL);
  assert_inner(!res, "traceview_titlebar_set_title");

  return 0;
}

int
traceview_titlebar_set_title (const char *title)
{
  int res = wattron(traceview_titlebar, COLOR_PAIR(1));
  assert_inner(res != ERR, "wattron");

  res = werase(traceview_titlebar);
  assert_inner(res != ERR, "werase");

  if (title == NULL)
    mvwprintw(traceview_titlebar, 0, 1, "graprof traceview");
  else
    mvwprintw(traceview_titlebar, 0, 1, "graprof traceview - %s", title);

  res = wrefresh(traceview_titlebar);
  assert_inner(res != ERR, "wrefresh");

  res = wattroff(traceview_titlebar, COLOR_PAIR(1));
  assert_inner(res != ERR, "wattroff");

  return 0;
}

#endif // HAVE_NCURSES
