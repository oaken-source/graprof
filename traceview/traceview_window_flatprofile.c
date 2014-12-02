
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


#include "traceview_window_flatprofile.h"

#if HAVE_NCURSES

#include <string.h>
#include <inttypes.h>

#include "../trace.h"
#include "../function.h"
#include "../strtime.h"

#include "traceview_scrollbar.h"
#include "traceview.h"

WINDOW *traceview_window_flatprofile = NULL;

static WINDOW *header = NULL;
static WINDOW *listing = NULL;

static unsigned int focus_index = 0;
static unsigned int focus_max = 0;
static unsigned int focus_function = 0;

static unsigned int scrolldown = 0;
static unsigned int scrolldown_max = 0;

static unsigned int items_shown = 0;

int
traceview_window_flatprofile_init (void)
{
  traceview_window_flatprofile = newwin(1, 1, 1, 1);
  assert_inner(traceview_window_flatprofile, "newwin");

  header = newwin(1, 1, 1, 1);
  assert_inner(header, "newwin");

  listing = newwin(1, 1, 1, 1);
  assert_inner(listing, "newwin");

  return 0;
}

static int __may_fail
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

  mvwprintw(header, 5, 2, " function list ");

  res = wrefresh(header);
  assert_inner(res != ERR, "wrefresh");

  return 0;
}

static int __may_fail
redraw_listing_item (unsigned int line, function *f, int focus)
{
  unsigned long long total_runtime = trace_get_total_runtime();
  float percent_time = (100.0 * f->self_time) / total_runtime;

  unsigned long long self_time = f->self_time;
  const char *self_time_prefix = strtime(&self_time);

  unsigned long long children_time = f->children_time;
  const char *children_time_prefix = strtime(&children_time);

  unsigned long long calls = f->calls;

  unsigned long long self_per_call = f->self_time / f->calls;
  const char *self_per_call_prefix = strtime(&self_per_call);

  unsigned long long children_per_call = f->children_time / f->calls;
  const char *children_per_call_prefix = strtime(&children_per_call);

  if (!strcmp(f->name, "??"))
    mvwprintw(listing, line, 0,
      " %6.2f %6llu %ss %6llu %ss %8llu %6llu %ss %6llu %ss  0x%" PRIxPTR,
      percent_time, self_time, self_time_prefix, children_time, children_time_prefix,
      calls, self_per_call, self_per_call_prefix, children_per_call, children_per_call_prefix,
      f->address);
  else
    mvwprintw(listing, line, 0,
      " %6.2f %6llu %ss %6llu %ss %8llu %6llu %ss %6llu %ss  %s",
      percent_time, self_time, self_time_prefix, children_time, children_time_prefix,
      calls, self_per_call, self_per_call_prefix, children_per_call, children_per_call_prefix,
      f->name);

  if (focus)
    {
      focus_function = (unsigned int)(f - function_get_all());
      int res = mvwchgat(listing, line, 0, -1, A_STANDOUT, 0, NULL);
      assert_inner(res != ERR, "mvwchgat");
    }

  return 0;
}

static int __may_fail
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
      "  %%       self    children             self    children");
  mvwprintw(listing, 2, 1,
      " time      time      time     calls    /call     /call  name");

  function **sorted_functions = function_get_all_sorted();

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
    }

  res = wrefresh(listing);
  assert_inner(res != ERR, "wrefresh");

  return 0;
}

int
traceview_window_flatprofile_redraw (void)
{
  int res = redraw_header();
  assert_inner(!res, "redraw_header");

  res = redraw_listing();
  assert_inner(!res, "redraw_listing");

  return 0;
}

int
traceview_window_flatprofile_key_dispatch (__unused traceview_key k)
{
  int res;

  switch (k)
    {
    case TRACEVIEW_KEY_ARROW_UP:
      focus_index = max(1, focus_index) - 1;
      res = traceview_window_flatprofile_redraw();
      assert_inner(!res, "traceview_window_flatprofile_redraw");
      break;

    case TRACEVIEW_KEY_ARROW_DOWN:
      focus_index = min((int)(focus_max - 1), (int)focus_index) + 1;
      res = traceview_window_flatprofile_redraw();
      assert_inner(!res, "traceview_window_flatprofile_redraw");
      break;

    case TRACEVIEW_KEY_PAGE_UP:
      focus_index = max(items_shown, focus_index) - items_shown;
      scrolldown = max(items_shown, scrolldown) - items_shown;
      res = traceview_window_flatprofile_redraw();
      assert_inner(!res, "traceview_window_flatprofile_redraw");
      break;

    case TRACEVIEW_KEY_PAGE_DOWN:
      focus_index = min((int)(focus_max - items_shown), (int)focus_index) + items_shown;
      scrolldown = min((int)(scrolldown_max - items_shown), (int)scrolldown) + items_shown;
      res = traceview_window_flatprofile_redraw();
      assert_inner(!res, "traceview_window_flatprofile_redraw");
      break;

    case TRACEVIEW_KEY_ENTER:
      res = traceview_navigate_to_callgraph_function(focus_function);
      assert_inner(!res, "traceview_navigate_to_callgraph_function");
      break;

    default:
      break;
    }
  return 0;
}

#endif // HAVE_NCURSES
