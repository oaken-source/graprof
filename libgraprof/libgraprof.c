
#include "libgraprof.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "highrestimer.h"
#include "mallhooks.h"
#include "instrument.h"

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

    //fprintf(libgraprof_out, "END %llu\n", highrestimer_get());
    
    unsigned long index = libgraprof_bufsize;
    libgraprof_bufsize += sizeof(char) + sizeof(unsigned long long);
    libgraprof_buf = realloc(libgraprof_buf, libgraprof_bufsize);
    *((char*)(libgraprof_buf + index)) = 'E';
    index += sizeof(char);
    *((unsigned long long*)(libgraprof_buf + index)) = highrestimer_get();

    fwrite(&libgraprof_bufsize, sizeof(unsigned long), 1, libgraprof_out);

    fwrite(libgraprof_buf, 1, libgraprof_bufsize, libgraprof_out);
    fclose(libgraprof_out);
  }

  errno = errsv;
}

