
#include "libgraprof.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "highrestimer.h"
#include "mallhooks.h"
#include "instrument.h"
#include "buffer.h"

FILE *libgraprof_out = NULL;
void *libgraprof_buf = NULL;
unsigned long libgraprof_bufsize = 0;

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

  if (libgraprof_out)
  {
    mallhooks_uninstall_hooks();

    buffer_enlarge(sizeof(char) + sizeof(unsigned long long));
    buffer_append(char, 'E');
    buffer_append(unsigned long long, highrestimer_get());

    fwrite(&libgraprof_bufsize, sizeof(unsigned long), 1, libgraprof_out);

    fwrite(libgraprof_buf, 1, libgraprof_bufsize, libgraprof_out);
    fclose(libgraprof_out);
  }

  errno = errsv;
}

