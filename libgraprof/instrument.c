
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013 - 2014  Andreas Grapentin                            *
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


#include "instrument.h"

#include "mallhooks.h"
#include "highrestimer.h"
#include "buffer.h"

#include <stdint.h>
#include <stdlib.h>

static unsigned int instrument_active = 0;

static void
instrument_enter (void *func, void *caller)
{
  unsigned long long time = highrestimer_get();

  mallhooks_uninstall_hooks();

  buffer_enlarge(sizeof(char) + 2 * sizeof(uintptr_t) + sizeof(unsigned long long));
  buffer_append(char, 'e');
  buffer_append(uintptr_t, (uintptr_t)func);
  buffer_append(uintptr_t, (uintptr_t)(caller - 4));
  buffer_append(unsigned long long, time);

  mallhooks_install_hooks();
}

static void
instrument_exit (void *func __attribute__ ((unused)), void *caller __attribute__ ((unused)))
{
  mallhooks_uninstall_hooks();

  buffer_enlarge(sizeof(char) + sizeof(unsigned long long));
  buffer_append(char, 'x');

  mallhooks_install_hooks();

  buffer_append(unsigned long long, highrestimer_get());
}

void
instrument_install_hooks (void)
{
  instrument_active = 1;
}

void
instrument_uninstall_hooks (void)
{
  instrument_active = 0;
}

void
__cyg_profile_func_enter (void *func, void *caller)
{
  if (instrument_active)
    instrument_enter(func, caller);
}

void
__cyg_profile_func_exit (void *func, void *caller)
{
  if (instrument_active)
    instrument_exit(func, caller);
}
