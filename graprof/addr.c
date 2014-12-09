
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
#include <grapes/vector.h>

static bfd *addr_bfd = NULL;
static asymbol **addr_syms = NULL;

struct addr_section
{
  asection *section;
  bfd_vma lower;
  bfd_vma upper;
};
typedef struct addr_section addr_section;

vector_declare(vec_addr_sections, addr_section);

static vec_addr_sections addr_sections = { 0 };

static int
addr_read_symbol_table (void)
{
  __precondition(ENOTSUP, bfd_get_file_flags(addr_bfd) & HAS_SYMS);

  int dynamic = 0;
  long storage = bfd_get_symtab_upper_bound(addr_bfd);
  if (!storage)
    {
      storage = bfd_get_dynamic_symtab_upper_bound(addr_bfd);
      dynamic = 1;
    }
  __precondition(ENOTSUP, storage > 0);

  __checked_call(NULL != (addr_syms = malloc(storage)));

  long symcount;
  if (dynamic)
    symcount = bfd_canonicalize_dynamic_symtab(addr_bfd, addr_syms);
  else
    symcount = bfd_canonicalize_symtab(addr_bfd, addr_syms);

  __precondition(ENOTSUP, symcount > 0);

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

      bfd_vma lower = bfd_get_section_vma(addr_bfd, s);
      bfd_vma upper = lower + bfd_get_section_size(s);
      addr_section a = { s, lower, upper };

      __checked_call(0 == vector_push(&addr_sections, a));
    }

  return 0;
}

int
addr_init (const char *filename)
{
  bfd_init();
  __checked_call(NULL != (addr_bfd = bfd_openr(filename, NULL)));

  addr_bfd->flags |= BFD_DECOMPRESS;

  __precondition(ENOTSUP, 0 != bfd_check_format(addr_bfd, bfd_object));

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

  vector_map(&addr_sections, ITEM,
    if (pc >= ITEM.lower && pc <= ITEM.upper)
      bfd_find_nearest_line_discriminator(addr_bfd, ITEM.section, addr_syms, pc - ITEM.lower, &_file, &_function, &_line, &_discriminator);
  );

  if (function)
    __checked_call(NULL != (*function = strdup(_function ? _function : "??")));

  if (file)
    __checked_call(NULL != (*file = strdup(_file ? _file : "??")));

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

  vector_clear(&addr_sections);
}

