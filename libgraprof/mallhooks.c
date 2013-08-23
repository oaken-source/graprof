
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013  Andreas Grapentin                                   *
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

#include "highrestimer.h"
#include "buffer.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int mallhooks_active = 0;

extern void* __libc_malloc(size_t size);
extern void* __libc_calloc(size_t nmemb, size_t size);
extern void* __libc_realloc(void *ptr, size_t size);
extern void __libc_free(void *ptr);

static void*
malloc_hook (size_t size, void *caller)
{
  void *result;

  mallhooks_uninstall_hooks();

  result = malloc(size);

  buffer_enlarge(sizeof(char) + sizeof(size_t) + 2 * sizeof(uintptr_t) + sizeof(unsigned long long));
  buffer_append(char, '+');
  buffer_append(size_t, size);
  buffer_append(uintptr_t, (uintptr_t)(caller - 4));
  buffer_append(uintptr_t, (uintptr_t)result);
  buffer_append(unsigned long long, highrestimer_get());

  mallhooks_install_hooks();

  return result;
}

static void*
calloc_hook (size_t nmemb, size_t size, void *caller)
{
  void *result;

  mallhooks_uninstall_hooks();

  result = calloc(nmemb, size);

  buffer_enlarge(sizeof(char) + sizeof(size_t) + 2 * sizeof(uintptr_t) + sizeof(unsigned long long));
  buffer_append(char, '+');
  buffer_append(size_t, nmemb * size);
  buffer_append(uintptr_t, (uintptr_t)(caller - 4));
  buffer_append(uintptr_t, (uintptr_t)result);
  buffer_append(unsigned long long, highrestimer_get());

  mallhooks_install_hooks();

  return result;
}

static void*
realloc_hook (void *ptr, size_t size, void *caller)
{
  void *result;

  mallhooks_uninstall_hooks();

  result = realloc(ptr, size);

  buffer_enlarge(sizeof(char) + sizeof(size_t) + 3 * sizeof(uintptr_t) + sizeof(unsigned long long));
  buffer_append(char, '*');
  buffer_append(uintptr_t, (uintptr_t)ptr);
  buffer_append(size_t, size);
  buffer_append(uintptr_t, (uintptr_t)(caller - 4));
  buffer_append(uintptr_t, (uintptr_t)result);
  buffer_append(unsigned long long, highrestimer_get());

  mallhooks_install_hooks();

  return result;
}

static void
free_hook (void *ptr, void *caller)
{
  mallhooks_uninstall_hooks();

  free(ptr);
 
  if (ptr)
    {
      buffer_enlarge(sizeof(char) + 2 * sizeof(uintptr_t) + sizeof(unsigned long long));
      buffer_append(char, '-');
      buffer_append(uintptr_t, (uintptr_t)ptr);
      buffer_append(uintptr_t, (uintptr_t)(caller - 4));
      buffer_append(unsigned long long, highrestimer_get());
    }

  mallhooks_install_hooks();
}

void
mallhooks_install_hooks ()
{
  mallhooks_active = 1;
}

void 
mallhooks_uninstall_hooks ()
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
