
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

WINDOW *traceview_window_flatprofile = NULL;

static int traceview_window_flatprofile_focus = -1;
static int traceview_window_flatprofile_max_focus = -1;

static int traceview_window_flatprofile_scrolldown = 0;
static int traceview_window_flatprofile_max_scrolldown = 0;

static unsigned int traceview_window_flatprofile_maxlines = 0;

int
traceview_window_flatprofile_init (void)
{
  traceview_window_flatprofile = newwin(LINES - 2, COLS, 1, 0);
  assert_inner(traceview_window_flatprofile, "newwin");

  return 0;
}

int
traceview_window_flatprofile_redraw (void)
{
  unsigned int nfunctions = function_get_nfunctions();
  traceview_window_flatprofile_maxlines = LINES - 2 - 9 - 2;

  traceview_window_flatprofile_max_scrolldown = nfunctions - traceview_window_flatprofile_maxlines;

  assert_set_errno(traceview_window_flatprofile_maxlines > 0, ENOTSUP, "terminal window too small");

  unsigned int have_scrollbar = (traceview_window_flatprofile_max_scrolldown > 0);
  int res = wresize(traceview_window_flatprofile, LINES - 2, COLS - have_scrollbar);
  assert_inner(res != ERR, "wresize");

  res = mvwin(traceview_window_flatprofile, 1, 0);
  assert_inner(res != ERR, "mvwin");

  res = werase(traceview_window_flatprofile);
  assert_inner(res != ERR, "werase");

  const char *prefix;
  unsigned long long time = trace_get_total_runtime();
  strtime(&time, &prefix);
  mvwprintw(traceview_window_flatprofile, 1, 1,
      "total runtime:                      %llu %sseconds", time, prefix);
  mvwprintw(traceview_window_flatprofile, 2, 1,
      "total number of function calls:     %llu", function_get_total_calls());
  mvwprintw(traceview_window_flatprofile, 3, 1,
      "total number of distinct functions: %u", nfunctions);

  traceview_window_flatprofile_max_focus = nfunctions - 1;

  res = mvwhline(traceview_window_flatprofile, 5, 0, 0, COLS);

  mvwprintw(traceview_window_flatprofile, 5, 2, " function list ");

  mvwprintw(traceview_window_flatprofile, 7, 1,
      "  %%       self    children             self    children");
  mvwprintw(traceview_window_flatprofile, 8, 1,
      " time      time      time     calls    /call     /call  name");

  function **sorted_functions = function_get_all_sorted(&nfunctions);
  function *functions = function_get_all(&nfunctions);

  unsigned int i;
  for (i = traceview_window_flatprofile_scrolldown; i < nfunctions; ++i)
    {
      if (i - traceview_window_flatprofile_scrolldown > traceview_window_flatprofile_maxlines)
        break;

      function *f = sorted_functions[i];

      unsigned long long total_runtime = trace_get_total_runtime();
      float percent_time = (100.0 * f->self_time) / total_runtime;

      unsigned long long self_time = f->self_time;
      const char *self_time_prefix = NULL;
      strtime(&self_time, &self_time_prefix);

      unsigned long long children_time = f->self_time;
      const char *children_time_prefix = NULL;
      strtime(&children_time, &children_time_prefix);

      unsigned long long calls = f->calls;

      unsigned long long self_per_call = f->self_time / f->calls;
      const char *self_per_call_prefix = NULL;
      strtime(&self_per_call, &self_per_call_prefix);

      unsigned long long children_per_call = f->children_time / f->calls;
      const char *children_per_call_prefix = NULL;
      strtime(&children_per_call, &children_per_call_prefix);

      unsigned int index = (unsigned int)(f - functions);

      if (!strcmp(f->name, "??"))
        mvwprintw(traceview_window_flatprofile, 9 + i - traceview_window_flatprofile_scrolldown, 1,
          "%6.2f %6llu %ss %6llu %ss %8lu %6llu %ss %6llu %ss  0x%" PRIxPTR " [%u]",
          percent_time, self_time, self_time_prefix, children_time, children_time_prefix,
          calls, self_per_call, self_per_call_prefix, children_per_call, children_per_call_prefix,
          f->address, index);
      else
        mvwprintw(traceview_window_flatprofile, 9 + i - traceview_window_flatprofile_scrolldown, 1,
          "%6.2f %6llu %ss %6llu %ss %8lu %6llu %ss %6llu %ss  %s [%u]",
          percent_time, self_time, self_time_prefix, children_time, children_time_prefix,
          calls, self_per_call, self_per_call_prefix, children_per_call, children_per_call_prefix,
          f->name, index);

      if ((int)i == traceview_window_flatprofile_focus)
        {
          res = mvwchgat(traceview_window_flatprofile, 9 + i - traceview_window_flatprofile_scrolldown, 0, COLS, A_STANDOUT, 0, NULL);
          assert_inner(res != ERR, "mvwchgat");
        }
    }

  if (have_scrollbar)
    {
      res = traceview_scrollbar_redraw(7, COLS - 1, LINES - 2 - 6, traceview_window_flatprofile_scrolldown * 1.0 / traceview_window_flatprofile_max_scrolldown);
      assert_inner(!res, "traceview_scrollbar_redraw");
    }

  res = wrefresh(traceview_window_flatprofile);
  assert_inner(res != ERR, "refresh");

  return 0;
}

int
traceview_window_flatprofile_key_dispatch (unused traceview_key k)
{
  switch (k)
    {
    case TRACEVIEW_KEY_ARROW_UP:
      if (traceview_window_flatprofile_focus > 0)
        {
          --traceview_window_flatprofile_focus;

          if (traceview_window_flatprofile_focus < traceview_window_flatprofile_scrolldown)
            traceview_window_flatprofile_scrolldown = traceview_window_flatprofile_focus;

          int res = traceview_window_flatprofile_redraw();
          assert_inner(!res, "traceview_window_flatprofile_redraw");
        }
      break;

    case TRACEVIEW_KEY_ARROW_DOWN:
      if (traceview_window_flatprofile_focus < traceview_window_flatprofile_max_focus)
        {
          ++traceview_window_flatprofile_focus;

          if (traceview_window_flatprofile_focus >= traceview_window_flatprofile_scrolldown + (int)traceview_window_flatprofile_maxlines)
            traceview_window_flatprofile_scrolldown = traceview_window_flatprofile_focus - traceview_window_flatprofile_maxlines;

          int res = traceview_window_flatprofile_redraw();
          assert_inner(!res, "traceview_window_flatprofile_redraw");
        }
      break;

    default:
      break;
    }
  return 0;
}

#endif // HAVE_NCURSES
