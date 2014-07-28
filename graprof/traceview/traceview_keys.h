
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


#pragma once

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#if HAVE_NCURSES

#include <grapes/util.h>

#include <ncurses.h>

enum traceview_key
{
  TRACEVIEW_KEY_NONE          = 0x0000,

  TRACEVIEW_KEY_QUIT          = 0x0001,
  TRACEVIEW_KEY_ENTER         = 0x0002,
  TRACEVIEW_KEY_BACKSPACE     = 0x0003,

  TRACEVIEW_KEY_ARROW_UP      = 0x1000,
  TRACEVIEW_KEY_ARROW_DOWN    = 0x1001,
  TRACEVIEW_KEY_ARROW_RIGHT   = 0x1002,
  TRACEVIEW_KEY_ARROW_LEFT    = 0x1003,

  TRACEVIEW_KEY_PAGE_UP       = 0x1004,
  TRACEVIEW_KEY_PAGE_DOWN     = 0x1005,

  TRACEVIEW_KEY_ESCAPE        = 0x1010,

  TRACEVIEW_KEY_F1            = 0x2001,
  TRACEVIEW_KEY_F2            = 0x2002,
  TRACEVIEW_KEY_F3            = 0x2003,
  TRACEVIEW_KEY_F4            = 0x2004,
  TRACEVIEW_KEY_F5            = 0x2005,
  TRACEVIEW_KEY_F6            = 0x2006,
  TRACEVIEW_KEY_F7            = 0x2007,
  TRACEVIEW_KEY_F8            = 0x2008,
  TRACEVIEW_KEY_F9            = 0x2009,
  TRACEVIEW_KEY_F10           = 0x2010,
  TRACEVIEW_KEY_F11           = 0x2011,
  TRACEVIEW_KEY_F12           = 0x2012,

  TRACEVIEW_KEY_ALT_1         = 0x8001,
  TRACEVIEW_KEY_ALT_2         = 0x8002,
  TRACEVIEW_KEY_ALT_3         = 0x8003,
  TRACEVIEW_KEY_ALT_4         = 0x8004,

};
typedef enum traceview_key traceview_key;

int traceview_keys_init(void) may_fail;

void traceview_keys_fini(void);

traceview_key traceview_keys_get(WINDOW *w);

#endif // HAVE_NCURSES
