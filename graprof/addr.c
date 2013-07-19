
#include "addr.h"

#include <config.h>
#include <bfd.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <grapes/util.h>

static bfd *addr_bfd = NULL;
static asymbol **addr_syms = NULL;

static int 
addr_read_symbol_table ()
{
  int res = bfd_get_file_flags(addr_bfd) & HAS_SYMS;
  assert_set_errno(res, ENOTSUP, "bfd_get_file_flags");

  int dynamic = 0;
  long storage = bfd_get_symtab_upper_bound(addr_bfd);
  if (!storage)
    {
      storage = bfd_get_dynamic_symtab_upper_bound(addr_bfd);
      dynamic = 1;
    }
  assert_set_errno(storage > 0, ENOTSUP, "bfd_get_symtab_upper_bounds");

  addr_syms = malloc(storage);
  assert_inner(addr_syms, "malloc");

  long symcount;
  if (dynamic)
    symcount = bfd_canonicalize_dynamic_symtab(addr_bfd, addr_syms);
  else
    symcount = bfd_canonicalize_symtab (addr_bfd, addr_syms);

  assert_set_errno(symcount > 0, ENOTSUP, "bfd_canonicalize_symtab");

  return 0;
}

static uintptr_t addr_pc;
static const char *addr_filename;
static const char *addr_functionname;
static unsigned int addr_line;
static unsigned int addr_discriminator;
static int addr_found;

/* Look for an address in a section.  This is called via
   bfd_map_over_sections.  */

static void
addr_find_in_section (bfd *abfd, asection *section, void *data __attribute__ ((unused)))
{
  bfd_vma vma;
  bfd_size_type size;

  if (addr_found)
    return;

  if (!(bfd_get_section_flags(abfd, section) & SEC_ALLOC))
    return;

  vma = bfd_get_section_vma(abfd, section);
  if (addr_pc < vma)
    return;

  size = bfd_get_section_size(section);
  if (addr_pc >= vma + size)
    return;

  addr_found = bfd_find_nearest_line_discriminator(abfd, section, addr_syms, addr_pc - vma, &addr_filename, &addr_functionname, &addr_line, &addr_discriminator);
}

int
addr_init (const char *filename)
{
  bfd_init();
  addr_bfd = bfd_openr(filename, NULL);
  assert_inner(addr_bfd, "bfd_openr");

  addr_bfd->flags |= BFD_DECOMPRESS;
 
  int res = bfd_check_format(addr_bfd, bfd_object);
  assert_set_errno(res, ENOTSUP, "bfd_check_format");

  res = addr_read_symbol_table();
  assert_inner(!res, "addr_read_symbol_table");

  return 0;
}

int 
addr_translate (uintptr_t pc, char **function, char **file, unsigned int *line)
{
  if (bfd_get_flavour(addr_bfd) == bfd_target_elf_flavour)
    {
      // TODO: understand
    }

  addr_pc = pc;
  addr_found = 0;
  bfd_map_over_sections(addr_bfd, addr_find_in_section, NULL);

  if (function)
    *function = NULL;
  if (file)
    *file = NULL;
  if (line)
    *line = 0;

  if (addr_found)
    {
      if (function)
        {
          *function = strdup(addr_functionname);
          assert_inner(*function, "strdup");
        }

      if (file)
        {
          *file = strdup(addr_filename);
          assert_inner(*file, "strdup");
        }

      if (line)
        *line = addr_line;
    }
    
  return addr_found;
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
