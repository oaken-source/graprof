
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

#if HAVE_NCURSES

#include <signal.h>
#include <string.h>
#include <locale.h>

#include <ncurses.h>

#include <grapes/feedback.h>

#include "traceview_titlebar.h"
#include "traceview_footer.h"

#include "traceview_window_interactive.h"
#include "traceview_window_flatprofile.h"
#include "traceview_window_callgraph.h"
#include "traceview_window_memprofile.h"

#include "traceview_help_overlay.h"
#include "traceview_scrollbar.h"

#include "traceview_keys.h"

typedef int (*traceview_key_dispatch_callback)(traceview_key);
typedef int (*traceview_redraw_callback)(void);

struct traceview_window
{
  unsigned int index;
  WINDOW *window;
  traceview_key_dispatch_callback key_dispatch;
  traceview_redraw_callback redraw;
  const char *title;
};
typedef struct traceview_window traceview_window;

static traceview_window traceview_windows[4];
static traceview_window *traceview_focus = NULL;

static unsigned int traceview_help_overlay_enabled = 0;


static int
traceview_init (void)
{
  setlocale(LC_CTYPE, "");

  WINDOW *w = initscr();
  assert_inner(w, "initscr");

  assert_inner(has_colors(), "has_colors");

  int res = start_color();
  assert_inner(res != ERR, "start_color");
  res = init_pair(1, COLOR_WHITE, COLOR_BLUE);
  assert_inner(res != ERR, "init_pair");

  res = cbreak();
  assert_inner(res != ERR, "cbreak");
  res = noecho();
  assert_inner(res != ERR, "noecho");
  res = curs_set(0);
  assert_inner(res != ERR, "curs_set");

  res = refresh();
  assert_inner(res != ERR, "refresh");

  res = traceview_titlebar_init();
  assert_inner(!res, "traceview_titlebar_init");

  res = traceview_footer_init();
  assert_inner(!res, "traceview_footer_init");

  res = traceview_window_interactive_init();
  assert_inner(!res, "traceview_window_interactive_init");

  res = traceview_window_flatprofile_init();
  assert_inner(!res, "traceview_window_flatprofile_init");

  res = traceview_window_callgraph_init();
  assert_inner(!res, "traceview_window_callgraph_init");

  res = traceview_window_memprofile_init();
  assert_inner(!res, "traceview_window_memprofile_init");

  res = traceview_help_overlay_init();
  assert_inner(!res, "traceview_help_overlay_init");

  res = traceview_scrollbar_init();
  assert_inner(!res, "traceview_scrollbar_init");

  traceview_windows[0].index = 1;
  traceview_windows[0].window = traceview_window_interactive;
  traceview_windows[0].key_dispatch = &traceview_window_interactive_key_dispatch;
  traceview_windows[0].redraw = &traceview_window_interactive_redraw;
  traceview_windows[0].title = "trace explorer";

  traceview_windows[1].index = 2;
  traceview_windows[1].window = traceview_window_flatprofile;
  traceview_windows[1].key_dispatch = &traceview_window_flatprofile_key_dispatch;
  traceview_windows[1].redraw = &traceview_window_flatprofile_redraw;
  traceview_windows[1].title = "flat runtime profile";

  traceview_windows[2].index = 3;
  traceview_windows[2].window = traceview_window_callgraph;
  traceview_windows[2].key_dispatch = &traceview_window_callgraph_key_dispatch;
  traceview_windows[2].redraw = &traceview_window_callgraph_redraw;
  traceview_windows[2].title = "callgraph profile";

  traceview_windows[3].index = 4;
  traceview_windows[3].window = traceview_window_memprofile;
  traceview_windows[3].key_dispatch = &traceview_window_memprofile_key_dispatch;
  traceview_windows[3].redraw = &traceview_window_memprofile_redraw;
  traceview_windows[3].title = "memory allocation profile";

  traceview_focus = traceview_windows;

  res = traceview_titlebar_set_title(traceview_focus->title);
  assert_inner(!res, "traceview_titlebar_set_title");

  res = traceview_footer_set_index(traceview_focus->index);
  assert_inner(!res, "traceview_footer_set_index");

  res = traceview_focus->redraw();
  assert_inner(!res, "traceview_focus->redraw");

  return 0;
}

static int
traceview_fini (void)
{
  endwin();

  return 0;
}

static int
traceview_main_inner (void)
{
  int res = traceview_init();
  assert_inner(!res, "traceview_init");

  do
    {
      traceview_key k = traceview_keys_get(traceview_focus->window);

      switch (k)
        {
        case TRACEVIEW_KEY_F1:
          if (traceview_help_overlay_enabled)
            {
              traceview_help_overlay_enabled = 0;
              res = traceview_focus->redraw();
              assert_inner(!res, "traceview_focus->redraw");
            }
          else
            {
              traceview_help_overlay_enabled = 1;
              res = traceview_help_overlay_redraw();
              assert_inner(!res, "traceview_help_overlay_redraw");
            }

          break;

        case TRACEVIEW_KEY_ALT_1:
        case TRACEVIEW_KEY_ALT_2:
        case TRACEVIEW_KEY_ALT_3:
        case TRACEVIEW_KEY_ALT_4:
          traceview_focus = traceview_windows + (k - TRACEVIEW_KEY_ALT_1);

          res = traceview_footer_set_index(traceview_focus->index);
          assert_inner(!res, "traceview_footer_set_index");
          res = traceview_titlebar_set_title(traceview_focus->title);
          assert_inner(!res, "traceview_titlebar_set_title");

          traceview_focus->redraw();
          assert_inner(res != ERR, "traceview_focus->redraw");

          if (traceview_help_overlay_enabled)
            {
              res = traceview_help_overlay_redraw();
              assert_inner(!res, "traceview_help_overlay_redraw");
            }

          break;

        case TRACEVIEW_KEY_ESCAPE:
        case TRACEVIEW_KEY_QUIT:
          if (traceview_help_overlay_enabled)
            {
              traceview_help_overlay_enabled = 0;
              res = traceview_focus->redraw();
              assert_inner(!res, "traceview_focus->redraw");
              break;
            }
          if (k == TRACEVIEW_KEY_ESCAPE)
            break;
          return 0;

        default:
          if (traceview_help_overlay_enabled)
            break;

          res = traceview_focus->key_dispatch(k);
          assert_inner(!res, "traceview_focus->key_dispatch");
          break;
        }
    }
  while (1);

  return 0;
}

static void
traceview_on_resize (unused int signum)
{
  int res = endwin();
  assert_weak(res != ERR, "endwin");
  res = refresh();
  assert_weak(res != ERR, "refresh");
  res = clear();
  assert_weak(res != ERR, "clear");

  res = traceview_titlebar_redraw();
  assert_weak(!res, "traceview_titlebar_redraw");
  res = traceview_footer_redraw();
  assert_weak(!res, "traceview_footer_redraw");

  res = traceview_focus->redraw();
  assert_weak(!res, "traceview_focus->redraw");

  if (traceview_help_overlay_enabled)
    {
      res = traceview_help_overlay_redraw();
      assert_weak(!res, "traceview_help_overlay_redraw");
    }
}

int
traceview_main (void)
{
  struct sigaction act;
  memset(&act, 0, sizeof(act));
  act.sa_handler = traceview_on_resize;

  int res = sigaction(SIGWINCH, &act, NULL);
  assert_inner(!res, "sigaction");

  int res1 = traceview_main_inner();
  int res2 = traceview_fini();
  assert_inner(!res1, "traceview_main_inner");
  assert_inner(!res2, "traceview_fini");

  return 0;
}

#else

int
traceview_main (void)
{
  errno = ENOSYS;
  return -1;
}

#endif // HAVE_NCURSES
