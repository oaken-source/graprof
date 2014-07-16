
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

static char
traceview_keys_get_immediate(WINDOW *w)
{
  int res = nodelay(w, 1);
  assert_inner(res != ERR, "nodelay");

  char k = wgetch(w);

  res = nodelay(w, 0);
  assert_inner(res != ERR, "nodelay");

  return k;
}

static traceview_key
traceview_keys_dispatch_0x1B_0x5B_0x31(WINDOW *w)
{
  char k = traceview_keys_get_immediate(w);

  char k2 = traceview_keys_get_immediate(w);
  if (k2 != 0x7E) // no F key
    return TRACEVIEW_KEY_NONE;

  switch (k)
    {
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
      return TRACEVIEW_KEY_F1 + (k - 0x31);

    case 0x37:
    case 0x38:
    case 0x39:
      return TRACEVIEW_KEY_F6 + (k - 0x37);

    default:
      fprintf(stderr, "0x1B, 0x5B, 0x31, 0x%02X\n", k);
      return TRACEVIEW_KEY_NONE;
    }
}

static traceview_key
traceview_keys_dispatch_0x1B_0x5B_0x32(WINDOW *w)
{
  char k = traceview_keys_get_immediate(w);

  char k2 = traceview_keys_get_immediate(w);
  if (k2 != 0x7E) // no F key
    return TRACEVIEW_KEY_NONE;

  switch (k)
    {
    case 0x30:
    case 0x31:
      return TRACEVIEW_KEY_F9 + (k - 0x30);

    case 0x33:
    case 0x34:
      return TRACEVIEW_KEY_F11 + (k - 0x33);

    default:
      fprintf(stderr, "0x1B, 0x5B, 0x32, 0x%02X\n", k);
      return TRACEVIEW_KEY_NONE;
    }
}

static traceview_key
traceview_keys_dispatch_0x1B_0x5B(WINDOW *w)
{
  char k = traceview_keys_get_immediate(w);

  switch (k)
    {
    case 0x31:
      return traceview_keys_dispatch_0x1B_0x5B_0x31(w); // F1 - F8

    case 0x32:
      return traceview_keys_dispatch_0x1B_0x5B_0x32(w); // F9 - F12

    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
      return TRACEVIEW_KEY_ARROW_UP + (k - 0x41);

    default:
      fprintf(stderr, "0x1B, 0x5B, 0x%02X\n", k);
      return TRACEVIEW_KEY_NONE;
    }
}

static traceview_key
traceview_keys_dispatch_0x1B(WINDOW *w)
{
  char k = traceview_keys_get_immediate(w);

  switch (k)
    {
    case 0x5B: // arrow keys / F keys / ...
      return traceview_keys_dispatch_0x1B_0x5B(w);

    case '1':
    case '2':
    case '3':
    case '4':
      return TRACEVIEW_KEY_ALT_1 + (k - '1');

    case ERR:
      return TRACEVIEW_KEY_ESCAPE;

    default:
      fprintf(stderr, "0x1B, 0x%02X\n", k);
      return TRACEVIEW_KEY_NONE;
    }
}

traceview_key
traceview_keys_get (WINDOW *w)
{
  char k = wgetch(w);

  switch (k)
    {
    case 0x1B: // escape / alt / arrow keys / F keys / ...
      return traceview_keys_dispatch_0x1B(w);

    case 0x0A: // enter
      return TRACEVIEW_KEY_ENTER;

    case 'q':
    case 'Q':
      return TRACEVIEW_KEY_QUIT;

    default:
      fprintf(stderr, "0x%02X\n", k);
      return TRACEVIEW_KEY_NONE;
    }
}

#endif // HAVE_NCURSES
