
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

#include <ncurses.h>

#include "traceview_titlebar.h"
#include "traceview_footer.h"

//#include "traceview_window_interactive.h"
//#include "traceview_window_flatprofile.h"
//#include "traceview_window_callgraph.h"
//#include "traceview_window_memprofile.h"

static WINDOW *traceview_focus = NULL;

//extern WINDOW *traceview_window_interactive;
//extern WINDOW *traceview_window_flatprofile;
//extern WINDOW *traceview_window_callgraph;
//extern WINDOW *traceview_window_memprofile;

static int
traceview_init (void)
{
  WINDOW *mainwin = initscr();
  assert_inner(mainwin, "initscr");

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

  res = wrefresh(mainwin);
  assert_inner(res != ERR, "refresh");

  res = traceview_titlebar_init();
  assert_inner(!res, "traceview_titlebar_init");

  res = traceview_footer_init();
  assert_inner(!res, "traceview_footer_init");

  //res = traceview_window_interactive_init();
  //assert_inner(!res, "traceview_window_interactive_init");

  //res = traceview_window_flatprofile_init();
  //assert_inner(!res, "traceview_window_flatprofile_init");

  //res = traceview_window_callgraph_init();
  //assert_inner(!res, "traceview_window_callgraph_init");

  //res = traceview_window_memprofile_init();
  //assert_inner(!res, "traceview_window_memprofile_init");

  //traceview_focus = traceview_window_interactive;

  traceview_focus = newwin(LINES - 2, COLS, 1, 0);

  res = wrefresh(traceview_focus);
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
      char c = wgetch(traceview_focus);
      switch (c)
        {
        case 27: // escape / alt
          {
            nodelay(traceview_focus, 1);
            char c2 = wgetch(traceview_focus);
            nodelay(traceview_focus, 0);

            switch (c2)
              {
                case '1':
                  //traceview_focus = traceview_window_interactive;
                  res = traceview_footer_set_index(1);
                  assert_inner(!res, "traceview_footer_set_index");
                  res = traceview_titlebar_set_title(NULL);
                  assert_inner(!res, "traceview_titlebar_set_title");
                  break;
                case '2':
                  res = traceview_footer_set_index(2);
                  assert_inner(!res, "traceview_footer_set_index");
                  res = traceview_titlebar_set_title("flat runtime profile");
                  assert_inner(!res, "traceview_titlebar_set_title");
                  //traceview_focus = traceview_window_flatprofile;
                  break;
                case '3':
                  res = traceview_footer_set_index(3);
                  assert_inner(!res, "traceview_footer_set_index");
                  res = traceview_titlebar_set_title("callgraph profile");
                  assert_inner(!res, "traceview_titlebar_set_title");
                  //traceview_focus = traceview_window_callgraph;
                  break;
                case '4':
                  res = traceview_footer_set_index(4);
                  assert_inner(!res, "traceview_footer_set_index");
                  res = traceview_titlebar_set_title("memory allocation profile");
                  assert_inner(!res, "traceview_titlebar_set_title");
                  //traceview_focus = traceview_window_memprofile;
                  break;
                default:
                  break;
              }

            res = wrefresh(traceview_focus);
            assert_inner(!res, "wrefresh");
          }
          break;
        case 'q':
        case 'Q':
          return 0;
        default:
          // TODO: dispatch
          break;
        }
    }
  while (1);

  return 0;
}

int
traceview_main (void)
{
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
