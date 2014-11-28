
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

  tracebuffer_packet p = {
    .type = '+',
    .malloc = { size, (uintptr_t)(caller - 4), (uintptr_t)result },
    .time = highrestimer_get()
  };
  tracebuffer_append(p);

  libgraprof_install_hooks();

  return result;
}

static void*
calloc_hook (size_t nmemb, size_t size, const void *caller)
{
  libgraprof_uninstall_hooks();

  void *result = calloc(nmemb, size);

  tracebuffer_packet p = {
    .type = '+',
    .calloc = { nmemb * size, (uintptr_t)(caller - 4), (uintptr_t)result },
    .time = highrestimer_get()
  };
  tracebuffer_append(p);

  libgraprof_install_hooks();

  return result;
}


static void*
realloc_hook (void *ptr, size_t size, const void *caller)
{
  libgraprof_uninstall_hooks();

  void *result = realloc(ptr, size);

  tracebuffer_packet p = {
    .type = '*',
    .realloc = { (uintptr_t)ptr, size, (uintptr_t)(caller - 4), (uintptr_t)result },
    .time = highrestimer_get()
  };
  tracebuffer_append(p);

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
      tracebuffer_packet p = {
        .type = '-',
        .free = { (uintptr_t)ptr, (uintptr_t)(caller - 4) },
        .time = highrestimer_get()
      };
      tracebuffer_append(p);
    }

  libgraprof_install_hooks();
}

#if USE_DEPRECATED_MALLOC_HOOKS

#include <malloc.h>

static void *(*old_malloc_hook)(size_t, const void*);
static void *(*old_realloc_hook)(void*, size_t, const void*);
static void (*old_free_hook)(void *, const void*);

void
libgraprof_install_hooks (void)
{
  old_malloc_hook = __malloc_hook;
  __malloc_hook = malloc_hook;

  old_realloc_hook = __realloc_hook;
  __realloc_hook = realloc_hook;

  old_free_hook = __free_hook;
  __free_hook = free_hook;
}

void
libgraprof_uninstall_hooks (void)
{
  __malloc_hook = old_malloc_hook;
  __realloc_hook = old_realloc_hook;
  __free_hook = old_free_hook;
}

#else // USE_DEPRECATED_MALLOC_HOOKS

extern void* __libc_malloc(size_t size);
extern void* __libc_calloc(size_t nmemb, size_t size);
extern void* __libc_realloc(void *ptr, size_t size);
extern void __libc_free(void *ptr);

static unsigned int mallhooks_active = 0;

void
mallhooks_install_hooks (void)
{
  mallhooks_active = 1;
}

void
mallhooks_uninstall_hooks (void)
{
  mallhooks_active = 0;
}

void*
malloc (size_t size)
{
  return (mallhooks_active ? malloc_hook(size, __builtin_return_address(0)) : __libc_malloc(size));
}

void*
calloc (size_t nmemb, size_t size)
{
  return (mallhooks_active ? calloc_hook(nmemb, size, __builtin_return_address(0)) : __libc_calloc(nmemb, size));
}

void*
realloc (void *ptr, size_t size)
{
  return (mallhooks_active ? realloc_hook(ptr, size, __builtin_return_address(0)) : __libc_realloc(ptr, size));
}

void
free (void *ptr)
{
  return (mallhooks_active ? free_hook(ptr, __builtin_return_address(0)) : __libc_free(ptr));
}

#endif // USE_DEPRECATED_MALLOC_HOOKS
