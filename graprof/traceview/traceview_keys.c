
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


#include "traceview_keys.h"

#include <grapes/util.h>

#if HAVE_NCURSES

static traceview_key
traceview_keys_dispatch_0x1B(WINDOW *w)
{
  int res = nodelay(w, 1);
  assert_weak(res != ERR, "nodelay");

  char k = wgetch(w);

  res = nodelay(w, 0);
  assert_weak(res != ERR, "nodelay");

  switch (k)
    {
    case '1':
    case '2':
    case '3':
    case '4':
      return TRACEVIEW_KEY_ALT_1 + (k - '1');

    default:
      return TRACEVIEW_KEY_NONE;
    }
}

traceview_key
traceview_keys_get (WINDOW *w)
{
  char k = wgetch(w);

  switch (k)
    {
    case 0x1B: // escape / alt / ...
      return traceview_keys_dispatch_0x1B(w);

    case 'q':
    case 'Q':
      return TRACEVIEW_KEY_QUIT;

    default:
      return TRACEVIEW_KEY_NONE;
    }
}

#endif // HAVE_NCURSES
