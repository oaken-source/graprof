
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2013 - 2014  Andreas Grapentin                            *
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

struct addr_section
{
  asection *section;
  bfd_vma lower;
  bfd_vma upper;
};
typedef struct addr_section addr_section;

addr_section *addr_sections = NULL;
unsigned int addr_nsections = 0;

static int
addr_read_symbol_table (void)
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

static int
addr_init_sections (void)
{
  asection *s;
  for (s = addr_bfd->sections; s != NULL; s = s->next)
    {
      if (!(bfd_get_section_flags(addr_bfd, s) & SEC_ALLOC))
        continue;

      ++addr_nsections;
      addr_sections = realloc(addr_sections, sizeof(*addr_sections) * addr_nsections);
      assert_inner(addr_sections, "realloc");

      addr_section *as = addr_sections + addr_nsections - 1;

      as->section = s;
      as->lower = bfd_get_section_vma(addr_bfd, s);
      as->upper = as->lower + bfd_get_section_size(s);
    }

  return 0;
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

  __checked_call(0 == addr_read_symbol_table());
  __checked_call(0 == addr_init_sections());

  return 0;
}

int
addr_translate (uintptr_t pc, char **function, char **file, unsigned int *line)
{
  const char *_function = NULL;
  const char *_file = NULL;
  unsigned int _line = 0;
  unsigned int _discriminator = 0;

  unsigned int i;
  for (i = 0; i < addr_nsections; ++i)
    if (pc >= addr_sections[i].lower && pc <= addr_sections[i].upper)
      bfd_find_nearest_line_discriminator(addr_bfd, addr_sections[i].section, addr_syms, pc - addr_sections[i].lower, &_file, &_function, &_line, &_discriminator);

  if (function)
    {
      *function = strdup(_function ? _function : "??");
      assert_inner(*function, "strdup");
    }

  if (file)
    {
      *file = strdup(_file ? _file : "??");
      assert_inner(*file, "strdup");
    }

  if (line)
    *line = _line;

  return 0;
}

static void
__attribute__ ((destructor))
addr_fini ()
{
  if (addr_bfd)
    bfd_close(addr_bfd);
  free(addr_syms);
  free(addr_sections);
}

