
#include "mallhooks.h"

#include "highrestimer.h"

#include <stdio.h>
#include <malloc.h>

extern FILE *libgraprof_out;

static void *mallhooks_malloc_hook(size_t, const void*);
static void *(*old_malloc_hook)(size_t, const void*);

static void *mallhooks_realloc_hook(void*, size_t, const void*);
static void *(*old_realloc_hook)(void*, size_t, const void*);

static void mallhooks_free_hook(void*, const void*);
static void (*old_free_hook)(void *, const void*);

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

static void*
mallhooks_malloc_hook (size_t size, const void *caller)
{
  void *result;

  mallhooks_uninstall_hooks();

  result = malloc(size);

  fprintf(libgraprof_out, "+ %u 0x%lx 0x%lx %llu\n", (unsigned int)size, (unsigned long)caller, (unsigned long)result, highrestimer_get());

  mallhooks_install_hooks();

  return result;
}

static void*
mallhooks_realloc_hook (void *ptr, size_t size, const void *caller)
{
  void *result;

  mallhooks_uninstall_hooks();

  result = realloc(ptr, size);

  fprintf(libgraprof_out, "* 0x%lx %u 0x%lx 0x%lx %llu\n", (unsigned long)ptr, (unsigned int)size, (unsigned long)caller, (unsigned long)result, highrestimer_get());

  mallhooks_install_hooks();

  return result;
}

static void
mallhooks_free_hook (void *ptr, const void *caller)
{
  mallhooks_uninstall_hooks();

  free(ptr);
 
  if (ptr)
    fprintf(libgraprof_out, "- 0x%lx 0x%lx %llu\n", (unsigned long)ptr, (unsigned long)caller, highrestimer_get());

  mallhooks_install_hooks();
}
