
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


#include "traceview_window_callgraph.h"

#if HAVE_NCURSES

#include "../function.h"
#include "../strtime.h"
#include "../trace.h"

WINDOW *traceview_window_callgraph = NULL;

static WINDOW *header = NULL;
static WINDOW *listing = NULL;

static unsigned int function_id = 0;

int
traceview_window_callgraph_init (void)
{
  traceview_window_callgraph = newwin(1, 1, 1, 1);
  assert_inner(traceview_window_callgraph, "newwin");

  header = newwin(1, 1, 1, 1);
  assert_inner(header, "newwin");

  listing = newwin(1, 1, 1, 1);
  assert_inner(listing, "newwin");

  return 0;
}

static int may_fail
redraw_header (void)
{
  int res = wresize(header, 6, COLS);
  assert_inner(res != ERR, "wresize");

  res = mvwin(header, 1, 0);
  assert_inner(res != ERR, "mvwin");

  res = werase(header);
  assert_inner(res != ERR, "werase");

  unsigned long long time = trace_get_total_runtime();
  const char *prefix = strtime(&time);
  mvwprintw(header, 1, 1,
      "total runtime:                      %llu %sseconds", time, prefix);
  mvwprintw(header, 2, 1,
      "total number of function calls:     %llu", function_get_total_calls());
  mvwprintw(header, 3, 1,
      "total number of distinct functions: %u", function_get_nfunctions());

  res = mvwhline(header, 5, 0, 0, COLS);

  function *f = function_get_all() + function_id;
  mvwprintw(header, 5, 2, " %s ", f->name);

  res = wrefresh(header);
  assert_inner(res != ERR, "wrefresh");

  return 0;
}

static int may_fail
redraw_listing (void)
{
  //function *f = function_get_all() + function_id;

  int res = wresize(listing, LINES - 8, COLS);
  assert_inner(res != ERR, "wresize");

  res = mvwin(listing, 7, 0);
  assert_inner(res != ERR, "mvwin");

  res = werase(listing);
  assert_inner(res != ERR, "werase");

  res = wrefresh(listing);
  assert_inner(res != ERR, "wrefresh");

  return 0;
}

int
traceview_window_callgraph_redraw (void)
{
  int res = redraw_header();
  assert_inner(!res, "redraw_header");

  res = redraw_listing();
  assert_inner(!res, "redraw_listing");

  return 0;
}

int
traceview_window_callgraph_key_dispatch (traceview_key k)
{
  int res;

  switch (k)
    {
      case TRACEVIEW_KEY_ARROW_RIGHT:
        fprintf(stderr, "arror right\n");
        function_id = min(function_get_nfunctions() - 1, function_id + 1);
        res = traceview_window_callgraph_redraw();
        assert_inner(!res, "traceview_window_callgraph_redraw");
        break;

      case TRACEVIEW_KEY_ARROW_LEFT:
        fprintf(stderr, "arror left\n");
        function_id = max(1, function_id) - 1;
        res = traceview_window_callgraph_redraw();
        assert_inner(!res, "traceview_window_callgraph_redraw");
        break;

      case TRACEVIEW_KEY_ENTER:
        // next function by selection
        break;

      case TRACEVIEW_KEY_BACKSPACE:
        // last viewed function
        break;

      default:
        break;
    }
  return 0;
}

int
traceview_window_callgraph_navigate_to_function (unsigned int _function_id)
{
  function_id = _function_id;

  int res = traceview_window_callgraph_redraw();
  assert_inner(!res, "traceview_window_callgraph_redraw");

  return 0;
}

#endif // HAVE_NCURSES
