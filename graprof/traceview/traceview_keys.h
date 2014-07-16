
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

#include <ncurses.h>

enum traceview_key
{
  TRACEVIEW_KEY_NONE      = 0x00,

  TRACEVIEW_KEY_ALT_1     = 0x01,
  TRACEVIEW_KEY_ALT_2     = 0x02,
  TRACEVIEW_KEY_ALT_3     = 0x03,
  TRACEVIEW_KEY_ALT_4     = 0x04,

  TRACEVIEW_KEY_QUIT      = 0x10
};
typedef enum traceview_key traceview_key;

traceview_key traceview_keys_get(WINDOW *w);

#endif // HAVE_NCURSES
