
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


#include "addr.h"

#include <bfd.h>

#include <stdlib.h>
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
    symcount = bfd_canonicalize_symtab(addr_bfd, addr_syms);

  assert_set_errno(symcount > 0, ENOTSUP, "bfd_canonicalize_symtab");

  return 0;
}

static uintptr_t addr_pc;
static const char *addr_filename;
static const char *addr_functionname;
static unsigned int addr_line;
static unsigned int addr_discriminator;
static int addr_found;

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
  addr_pc = pc;
  addr_found = 0;

  if (addr_bfd)
    bfd_map_over_sections(addr_bfd, addr_find_in_section, NULL);

  if (function)
    {
      *function = strdup(addr_found ? addr_functionname : "??");
      assert_inner(*function, "strdup");
    }

  if (file)
    {
      *file = strdup(addr_found ? addr_filename : "??");
      assert_inner(*file, "strdup");
    }

  if (line)
    *line = (addr_found ? addr_line : 0);
    
  return 0;
}

static void
__attribute__ ((destructor))
addr_fini ()
{
  if (addr_bfd)
    bfd_close(addr_bfd);
  if (addr_syms)
    free(addr_syms);
}
