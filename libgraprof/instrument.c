
#include "instrument.h"

#include "mallhooks.h"
#include "highrestimer.h"

#include <stdio.h>

extern FILE *libgraprof_out;

void
__cyg_profile_func_enter (void *func, void *caller)
{
  if (libgraprof_out)
    {
      mallhooks_uninstall_hooks();
      fprintf(libgraprof_out, "e 0x%lx 0x%lx %lu\n", (unsigned long)func, (unsigned long)caller, highrestimer_get());
      mallhooks_install_hooks();
    }
}

void
__cyg_profile_func_exit (void *func, void *caller)
{
  if (libgraprof_out)
    {
      mallhooks_uninstall_hooks();
      fprintf(libgraprof_out, "x 0x%lx 0x%lx %lu\n", (unsigned long)func, (unsigned long)caller, highrestimer_get());
      mallhooks_install_hooks();
    }
}

