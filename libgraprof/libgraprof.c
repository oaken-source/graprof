
#include "libgraprof.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "mallhooks.h"

FILE *libgraprof_out;

void 
__attribute__ ((constructor))
instrument_begin ()
{
  int errsv = errno;

  libgraprof_out = NULL;
  char *file = getenv("GRAPROF_OUT");

  if (file)
    {
      libgraprof_out = fopen(file, "w");
      mallhooks_install_hooks();
    }

  errno = errsv;
}

void
__attribute__ ((destructor))
instrument_end()
{
  int errsv = errno;

  mallhooks_uninstall_hooks();

  if (libgraprof_out)
    fclose(libgraprof_out);

  errno = errsv;
}

