
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


#include "traceview.h"

#include <grapes/feedback.h>

#include <ncurses.h>

static WINDOW *traceview_window = NULL;

static int
traceview_init (void)
{
  traceview_window = initscr();
  assert_inner(traceview_window, "initscr");

  return 0;
}

static int
traceview_fini (void)
{
  endwin();

  return 0;
}

int
traceview_main (void)
{
  int res = traceview_init();
  assert_inner(!res, "traceview_init");

  res = traceview_fini();
  assert_inner(!res, "traceview_fini");

  return 0;
}

