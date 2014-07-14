
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

#include <ncurses.h>

#include <grapes/feedback.h>

#include "traceview_titlebar.h"
#include "traceview_footer.h"

#include "traceview_window_interactive.h"
#include "traceview_window_flatprofile.h"
#include "traceview_window_callgraph.h"
#include "traceview_window_memprofile.h"

extern WINDOW *traceview_window_interactive;
extern WINDOW *traceview_window_flatprofile;
extern WINDOW *traceview_window_callgraph;
extern WINDOW *traceview_window_memprofile;

typedef int (*traceview_key_dispatch)(char);

struct traceview_window
{
  unsigned int index;
  WINDOW *window;
  traceview_key_dispatch dispatch_callback;
  const char *title;
};
typedef struct traceview_window traceview_window;

static traceview_window traceview_windows[4];

static traceview_window *traceview_focus = NULL;

static WINDOW *traceview_mainwindow = NULL;


static int
traceview_init (void)
{
  traceview_mainwindow = initscr();
  assert_inner(traceview_mainwindow, "initscr");

  assert_inner(has_colors(), "has_colors");

  int res = start_color();
  assert_inner(res != ERR, "start_color");
  res = init_pair(1, COLOR_WHITE, COLOR_BLUE);
  assert_inner(res != ERR, "init_pair");

  res = cbreak();
  assert_inner(res != ERR, "cbreak");
  res = noecho();
  assert_inner(res != ERR, "noecho");
  res = keypad(stdscr, 1);
  assert_inner(res != ERR, "keypad");
  res = curs_set(0);
  assert_inner(res != ERR, "curs_set");

  res = wrefresh(traceview_mainwindow);
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

  traceview_windows[0].index = 1;
  traceview_windows[0].window = traceview_window_interactive;
  traceview_windows[0].dispatch_callback = traceview_window_interactive_key_dispatch;
  traceview_windows[0].title = "trace explorer";

  traceview_windows[1].index = 2;
  traceview_windows[1].window = traceview_window_flatprofile;
  traceview_windows[1].dispatch_callback = traceview_window_flatprofile_key_dispatch;
  traceview_windows[1].title = "flat runtime profile";

  traceview_windows[2].index = 3;
  traceview_windows[2].window = traceview_window_callgraph;
  traceview_windows[2].dispatch_callback = traceview_window_callgraph_key_dispatch;
  traceview_windows[2].title = "callgraph profile";

  traceview_windows[3].index = 4;
  traceview_windows[3].window = traceview_window_memprofile;
  traceview_windows[3].dispatch_callback = traceview_window_memprofile_key_dispatch;
  traceview_windows[3].title = "memory allocation profile";

  traceview_focus = traceview_windows;

  res = traceview_titlebar_set_title(traceview_focus->title);
  assert_inner(!res, "traceview_titlebar_set_title");

  res = traceview_footer_set_index(traceview_focus->index);
  assert_inner(!res, "traceview_footer_set_index");

  res = wrefresh(traceview_focus->window);
  assert_inner(res != ERR, "wrefresh");

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
      char c = wgetch(traceview_focus->window);
      switch (c)
        {
        case 27: // escape / alt
          {
            nodelay(traceview_focus->window, 1);
            char c2 = wgetch(traceview_focus->window);
            nodelay(traceview_focus->window, 0);

            if (c2 >= '1' && c2 <= '4')
              {
                traceview_focus = traceview_windows + (c2 - '1');

                res = traceview_footer_set_index(traceview_focus->index);
                assert_inner(!res, "traceview_footer_set_index");
                res = traceview_titlebar_set_title(traceview_focus->title);
                assert_inner(!res, "traceview_titlebar_set_title");

                res = wrefresh(traceview_focus->window);
                assert_inner(res != ERR, "wrefresh");
              }
          }
          break;
        case 'q': case 'Q':
          return 0;
        default:
          res = traceview_focus->dispatch_callback(c);
          assert_inner(!res, "traceview_focus->dispatch_callback");
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

  res = wrefresh(traceview_mainwindow);
  assert_weak(res != ERR, "wrefresh");

  res = traceview_titlebar_redraw();
  assert_weak(!res, "traceview_titlebar_redraw");
  res = traceview_footer_redraw();
  assert_weak(!res, "traceview_footer_redraw");
  res = traceview_window_interactive_redraw();
  assert_weak(!res, "traceview_window_interactive_redraw");
  res = traceview_window_flatprofile_redraw();
  assert_weak(!res, "traceview_window_flatprofile_redraw");
  res = traceview_window_callgraph_redraw();
  assert_weak(!res, "traceview_window_callgraph_redraw");
  res = traceview_window_memprofile_redraw();
  assert_weak(!res, "traceview_window_memprofile_redraw");

  res = wrefresh(traceview_focus->window);
  assert_weak(res != ERR, "wrefresh");
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
