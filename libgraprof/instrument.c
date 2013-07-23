
#include "instrument.h"

#include "mallhooks.h"
#include "highrestimer.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

extern void *libgraprof_buf;
extern unsigned long libgraprof_bufsize;

static void __cyg_profile_func_enter_impl(void *, void*);
static void __cyg_profile_func_exit_impl(void*, void*);

static void instrument_nop(void*, void*);

static void (*__cyg_profile_func_enter_ptr)(void*, void*) = &instrument_nop;
static void (*__cyg_profile_func_exit_ptr)(void*, void*) = &instrument_nop;

static void
__cyg_profile_func_enter_impl (void *func, void *caller)
{
  unsigned long long time = highrestimer_get();

  mallhooks_uninstall_hooks();

  unsigned long index = libgraprof_bufsize;
  libgraprof_bufsize += sizeof(char) + 2 * sizeof(uintptr_t) + sizeof(unsigned long long);
  libgraprof_buf = realloc(libgraprof_buf, libgraprof_bufsize);
  *((char*)(libgraprof_buf + index)) = 'e';
  index += sizeof(char);
  *((uintptr_t*)(libgraprof_buf + index)) = (uintptr_t)func;
  index += sizeof(uintptr_t);
  *((uintptr_t*)(libgraprof_buf + index)) = (uintptr_t)(caller - 4);
  index += sizeof(uintptr_t);
  *((unsigned long long*)(libgraprof_buf + index)) = time;

  // fprintf(libgraprof_out, "e 0x%" PRIxPTR " 0x%" PRIxPTR " %llu\n", (uintptr_t)func, (uintptr_t)caller - 4, highrestimer_get());

  mallhooks_install_hooks();
}

static void
__cyg_profile_func_exit_impl (void *func __attribute__ ((unused)), void *caller __attribute__ ((unused)))
{
  mallhooks_uninstall_hooks();

  unsigned long index = libgraprof_bufsize;
  libgraprof_bufsize += sizeof(char) + sizeof(unsigned long long);
  libgraprof_buf = realloc(libgraprof_buf, libgraprof_bufsize);
  *((char*)(libgraprof_buf + index)) = 'x';
  index += sizeof(char);
  
  // fprintf(libgraprof_out, "x 0x%" PRIxPTR " 0x%" PRIxPTR " %llu\n", (uintptr_t)func, (uintptr_t)caller - 4, highrestimer_get());

  mallhooks_install_hooks();
  
  *((unsigned long long*)(libgraprof_buf + index)) = highrestimer_get();
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
instrument_uninstall_hooks ()
{
  __cyg_profile_func_enter_ptr = &instrument_nop;
  __cyg_profile_func_exit_ptr = &instrument_nop;
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
