
#include "libgraprof.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "mallhooks.h"
#include "instrument.h"

FILE *libgraprof_out = NULL;

void 
__attribute__ ((constructor))
libgraprof_init ()
{
  int errsv = errno;

  char *filename = getenv("GRAPROF_OUT");

  if (filename)
    {
      libgraprof_out = fopen(filename, "w");
    }

  if (libgraprof_out)
    {
      mallhooks_install_hooks();
      instrument_install_hooks();
    }

  errno = errsv;
}

void
__attribute__ ((destructor))
libgraprof_fini()
{
  int errsv = errno;

  mallhooks_uninstall_hooks();

  if (libgraprof_out)
    fclose(libgraprof_out);

  errno = errsv;
}

