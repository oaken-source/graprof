
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

#include "libgraprof.h"
#include "highrestimer.h"

#include "common/tracebuffer.h"

#include <stdint.h>
#include <stdlib.h>

void
__cyg_profile_func_enter (void *func, void *caller)
{
  if (__unlikely(!libgraprof_hooked))
    return;

  libgraprof_uninstall_hooks();

  unsigned long long time = highrestimer_get();

  tracebuffer_packet p = {
    .type = 'e',
    .enter = { (uintptr_t)func, (uintptr_t)(caller - 4) },
    .time = time
  };
  tracebuffer_append(p);

  libgraprof_install_hooks();
}

void
__cyg_profile_func_exit (__unused void *func, __unused void *caller)
{
  if (__unlikely(!libgraprof_hooked))
    return;

  libgraprof_uninstall_hooks();

  tracebuffer_packet p = {
    .type = 'x',
    .exit = { },
    .time = highrestimer_get()
  };
  tracebuffer_append(p);

  libgraprof_install_hooks();
}
