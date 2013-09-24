
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


#include "libgraprof.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <openssl/md5.h>

#include "highrestimer.h"
#include "mallhooks.h"
#include "instrument.h"
#include "buffer.h"

FILE *libgraprof_out = NULL;
void *libgraprof_buf = NULL;
unsigned long libgraprof_bufsize = 0;

static void
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

static void
__attribute__ ((destructor))
libgraprof_fini ()
{
  int errsv = errno;

  if (libgraprof_out)
  {
    mallhooks_uninstall_hooks();

    buffer_enlarge(sizeof(char) + sizeof(unsigned long long));
    buffer_append(char, 'E');
    buffer_append(unsigned long long, highrestimer_get());

    unsigned char md5[MD5_DIGEST_LENGTH] = { 0 };
    FILE *binary = fopen("/proc/self/exe", "r");
    if (!binary) 
      {
        perror("libgraprof: warning: binary identification impossible: /proc/self/exe");
      }
    else
      {
        MD5_CTX c;
        MD5_Init(&c);
    
        unsigned char buf[64 * 1024];
        
        size_t n;
        while ((n = fread(buf, 1, 64 * 1024, binary)))
          MD5_Update(&c, buf, n);
        MD5_Final(md5, &c);
       
        fclose(binary);
      }

    size_t res;
    res = fwrite(md5, 1, MD5_DIGEST_LENGTH, libgraprof_out);
    if (res != MD5_DIGEST_LENGTH)
      perror("libgraprof: error: writing trace file");

    res = fwrite(&libgraprof_bufsize, sizeof(unsigned long), 1, libgraprof_out);
    if (res != 1)
      perror("libgraprof: error: writing trace file");

    res = fwrite(libgraprof_buf, 1, libgraprof_bufsize, libgraprof_out);
    if (res != libgraprof_bufsize)
      perror("libgraprof: error: writing trace file");

    fclose(libgraprof_out);
  }

  buffer_destroy();
  errno = errsv;
}
