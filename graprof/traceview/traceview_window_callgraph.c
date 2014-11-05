
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

static unsigned int focus_index = 0;
static unsigned int focus_max = 0;
unused static unsigned int focus_function = 0;

static unsigned int scrolldown = 0;
static unsigned int scrolldown_max = 0;

static unsigned int items_shown = 0;

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

  function *f = function_get_all() + function_id;
  unsigned long long total_time = f->self_time + f->children_time;
  const char *total_time_prefix = strtime(&total_time);
  unsigned long long self_time = f->self_time;
  const char *self_time_prefix = strtime(&self_time);


  mvwprintw(header, 1, 1,
      "function name:                      %s", f->name);
  mvwprintw(header, 2, 1,
      "total time:                         %llu %sseconds (%.2f%%)", total_time, total_time_prefix);
  mvwprintw(header, 3, 1,
      "self time:                          %llu %sseconds (%.2f%%)", self_time, self_time_prefix);

  res = mvwhline(header, 5, 0, 0, COLS);

  mvwprintw(header, 5, 2, " %s ", f->name);

  res = wrefresh(header);
  assert_inner(res != ERR, "wrefresh");

  return 0;
}

static int may_fail
redraw_listing (void)
{
  unsigned int height = LINES - 8;
  items_shown = height - 3;
  unsigned int items_total = function_get_nfunctions();

  unsigned int have_scrollbar = (items_shown < items_total);
  focus_max = items_total - 1;
  scrolldown_max = items_total - items_shown;

  if (focus_index < scrolldown)
    scrolldown = focus_index;
  if (focus_index >= scrolldown + items_shown)
    scrolldown = focus_index - items_shown + 1;

  int res = wresize(listing, height, COLS - have_scrollbar);
  assert_inner(res != ERR, "wresize");

  res = mvwin(listing, 7, 0);
  assert_inner(res != ERR, "mvwin");

  res = werase(listing);
  assert_inner(res != ERR, "werase");

  mvwprintw(listing, 1, 1,
      "  %%       self    children");
  mvwprintw(listing, 2, 1,
      " time      time      time       called       name");

  /*function **sorted_functions = function_get_all_sorted();

  unsigned int i;
  for (i = scrolldown; i < min(items_total, scrolldown + items_shown); ++i)
    {
      res = redraw_listing_item(3 + i - scrolldown, sorted_functions[i], i == focus_index);
      assert_inner(!res, "redraw_listing_item");
    }

  if (have_scrollbar)
    {
      res = traceview_scrollbar_redraw(7, COLS - 1, height, scrolldown * 1.0 / scrolldown_max);
      assert_inner(!res, "traceview_scrollbar_redraw");
    }*/

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

      case TRACEVIEW_KEY_ARROW_DOWN:

        break;

      case TRACEVIEW_KEY_ARROW_UP:

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
