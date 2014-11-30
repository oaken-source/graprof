
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


#include "mallhooks.h"

#include "libgraprof.h"
#include "highrestimer.h"

#include "common/tracebuffer.h"

#include <stdint.h>
#include <stdlib.h>

static void*
malloc_hook (size_t size, const void *caller)
{
  libgraprof_uninstall_hooks();

  void *result = malloc(size);

  static tracebuffer_packet p = { .type = '+' };
    p.malloc.size   = size;
    p.malloc.caller = (uintptr_t)(caller - 4);
    p.malloc.result = (uintptr_t)result;
    p.time = highrestimer_get();
  tracebuffer_append(&p);

  libgraprof_install_hooks();

  return result;
}

static void*
calloc_hook (size_t nmemb, size_t size, const void *caller)
{
  libgraprof_uninstall_hooks();

  void *result = calloc(nmemb, size);

  static tracebuffer_packet p = { .type = '+' };
    p.calloc.size   = nmemb * size;
    p.calloc.caller = (uintptr_t)(caller - 4);
    p.calloc.result = (uintptr_t)result;
    p.time = highrestimer_get();
  tracebuffer_append(&p);

  libgraprof_install_hooks();

  return result;
}


static void*
realloc_hook (void *ptr, size_t size, const void *caller)
{
  libgraprof_uninstall_hooks();

  void *result = realloc(ptr, size);

  static tracebuffer_packet p = { .type = '*' };
    p.realloc.ptr     = (uintptr_t)ptr;
    p.realloc.size    = size;
    p.realloc.caller  = (uintptr_t)(caller - 4);
    p.realloc.result  = (uintptr_t)result;
    p.time = highrestimer_get();
  tracebuffer_append(&p);

  libgraprof_install_hooks();

  return result;
}

static void
free_hook (void *ptr, const void *caller)
{
  libgraprof_uninstall_hooks();

  free(ptr);

  if (ptr)
    {
      static tracebuffer_packet p = { .type = '-' };
        p.free.ptr    = (uintptr_t)ptr;
        p.free.caller = (uintptr_t)(caller - 4);
        p.time = highrestimer_get();
      tracebuffer_append(&p);
    }

  libgraprof_install_hooks();
}

extern void* __libc_malloc(size_t size);
extern void* __libc_calloc(size_t nmemb, size_t size);
extern void* __libc_realloc(void *ptr, size_t size);
extern void __libc_free(void *ptr);

void*
malloc (size_t size)
{
  return (libgraprof_hooked ? malloc_hook(size, __builtin_return_address(0)) : __libc_malloc(size));
}

void*
calloc (size_t nmemb, size_t size)
{
  return (libgraprof_hooked ? calloc_hook(nmemb, size, __builtin_return_address(0)) : __libc_calloc(nmemb, size));
}

void*
realloc (void *ptr, size_t size)
{
  return (libgraprof_hooked ? realloc_hook(ptr, size, __builtin_return_address(0)) : __libc_realloc(ptr, size));
}

void
free (void *ptr)
{
  return (libgraprof_hooked ? free_hook(ptr, __builtin_return_address(0)) : __libc_free(ptr));
}
