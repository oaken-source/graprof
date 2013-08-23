
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
#include <malloc.h>

static void *mallhooks_malloc_hook(size_t, const void*);
static void *(*old_malloc_hook)(size_t, const void*);

static void *mallhooks_realloc_hook(void*, size_t, const void*);
static void *(*old_realloc_hook)(void*, size_t, const void*);

static void mallhooks_free_hook(void*, const void*);
static void (*old_free_hook)(void *, const void*);

static void*
mallhooks_malloc_hook (size_t size, const void *caller)
{
  void *result;

  mallhooks_uninstall_hooks();

  result = malloc(size);

  buffer_enlarge(sizeof(char) + sizeof(size_t) + 2 * sizeof(uintptr_t) + sizeof(unsigned long long));
  buffer_append(char, '+');
  buffer_append(size_t, size);
  buffer_append(uintptr_t, (uintptr_t)caller);
  buffer_append(uintptr_t, (uintptr_t)result);
  buffer_append(unsigned long long, highrestimer_get());

  mallhooks_install_hooks();

  return result;
}

static void*
mallhooks_realloc_hook (void *ptr, size_t size, const void *caller)
{
  void *result;

  mallhooks_uninstall_hooks();

  result = realloc(ptr, size);

  buffer_enlarge(sizeof(char) + sizeof(size_t) + 3 * sizeof(uintptr_t) + sizeof(unsigned long long));
  buffer_append(char, '*');
  buffer_append(uintptr_t, (uintptr_t)ptr);
  buffer_append(size_t, size);
  buffer_append(uintptr_t, (uintptr_t)caller);
  buffer_append(uintptr_t, (uintptr_t)result);
  buffer_append(unsigned long long, highrestimer_get());

  mallhooks_install_hooks();

  return result;
}

static void
mallhooks_free_hook (void *ptr, const void *caller)
{
  mallhooks_uninstall_hooks();

  free(ptr);
 
  if (ptr)
    {
      buffer_enlarge(sizeof(char) + 2 * sizeof(uintptr_t) + sizeof(unsigned long long));
      buffer_append(char, '-');
      buffer_append(uintptr_t, (uintptr_t)ptr);
      buffer_append(uintptr_t, (uintptr_t)caller);
      buffer_append(unsigned long long, highrestimer_get());
    }

  mallhooks_install_hooks();
}

void 
mallhooks_install_hooks ()
{
  old_malloc_hook = __malloc_hook;
  __malloc_hook = mallhooks_malloc_hook;

  old_realloc_hook = __realloc_hook;
  __realloc_hook = mallhooks_realloc_hook;

  old_free_hook = __free_hook;
  __free_hook = mallhooks_free_hook;
}

void mallhooks_uninstall_hooks ()
{
  __malloc_hook = old_malloc_hook;
  __realloc_hook = old_realloc_hook;
  __free_hook = old_free_hook;
}
