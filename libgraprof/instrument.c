
#include "instrument.h"

#include "mallhooks.h"
#include "highrestimer.h"

#include <inttypes.h>
#include <stdio.h>

extern FILE *libgraprof_out;

static void __cyg_profile_func_enter_impl(void *, void*);
static void __cyg_profile_func_exit_impl(void*, void*);

static void instrument_nop(void*, void*);

static void (*__cyg_profile_func_enter_ptr)(void*, void*) = &instrument_nop;
static void (*__cyg_profile_func_exit_ptr)(void*, void*) = &instrument_nop;

static void
__cyg_profile_func_enter_impl (void *func, void *caller)
{
  mallhooks_uninstall_hooks();
  fprintf(libgraprof_out, "e 0x%" PRIxPTR " 0x%" PRIxPTR " %llu\n", (uintptr_t)func, (uintptr_t)caller - 4, highrestimer_get());
  mallhooks_install_hooks();
}

static void
__cyg_profile_func_exit_impl (void *func, void *caller)
{
  mallhooks_uninstall_hooks();
  fprintf(libgraprof_out, "x 0x%" PRIxPTR " 0x%" PRIxPTR " %llu\n", (uintptr_t)func, (uintptr_t)caller - 4, highrestimer_get());
  mallhooks_install_hooks();
}

static void
instrument_nop (void *func __attribute__ ((unused)), void *caller __attribute__ ((unused)))
{
  return;
}

void
instrument_install_hooks ()
{
  __cyg_profile_func_enter_ptr = &__cyg_profile_func_enter_impl;
  __cyg_profile_func_exit_ptr = &__cyg_profile_func_exit_impl;
}

void
__cyg_profile_func_enter (void *func, void *caller)
{
  (*__cyg_profile_func_enter_ptr)(func, caller);
}

void
__cyg_profile_func_exit (void *func, void *caller)
{
  (*__cyg_profile_func_exit_ptr)(func, caller);
}
