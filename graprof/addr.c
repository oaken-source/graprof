
#include "addr.h"

#include <config.h>
#include <bfd.h>

#include <stdlib.h>
#include <stdio.h>

#include <grapes/util.h>

static bfd *addr_bfd = NULL;
static asymbol **addr_syms = NULL;

int
addr_init (const char *filename)
{
  bfd_init();
  addr_bfd = bfd_openr(filename, NULL);
  assert_inner(addr_bfd, "bfd_openr");

  addr_bfd->flags |= BFD_DECOMPRESS;
 
  int res = bfd_check_format(addr_bfd, bfd_object);
  assert_set_errno(res, ENOTSUP, "bfd_check_format");

  res = bfd_get_file_flags(addr_bfd) & HAS_SYMS;
  assert_set_errno(res, ENOTSUP, "bfd_get_file_flags");

  long storage = bfd_get_symtab_upper_bound(addr_bfd);
  int dynamic = 0;
  if (!storage)
    {
      storage = bfd_get_dynamic_symtab_upper_bound(addr_bfd);
      dynamic = 1;
    }
  assert_set_errno(storage >= 0, ENOTSUP, "bfd_get_symtab_upper_bound");

  addr_syms = malloc(storage);
  assert_inner(addr_syms, "malloc");

  long symcount;
  if (dynamic)
    symcount = bfd_canonicalize_dynamic_symtab(addr_bfd, addr_syms);
  else 
    symcount = bfd_canonicalize_symtab(addr_bfd, addr_syms);

  assert_set_errno(symcount > 0, ENOTSUP, "bfd_canonicalize_symtab");

  return 0;
}

const char* 
addr_translate (unsigned long pc)
{
  pc = pc;

  return NULL;
}

void
__attribute__ ((destructor))
addr_fini ()
{
  if (addr_bfd)
    bfd_close(addr_bfd);
  if (addr_syms)
    free(addr_syms);
}
