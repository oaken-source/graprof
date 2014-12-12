
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

struct addr_symbol
{
  bfd_vma lower;
  bfd_vma upper;
  const char *name;
};
typedef struct addr_symbol addr_symbol;

vector_declare(vec_symbols, addr_symbol);

struct addr_symtab
{
  bfd *bfd;
  bfd_vma lower;
  bfd_vma upper;
  vec_symbols symbols;
};
typedef struct addr_symtab addr_symtab;

vector_declare(vec_symtabs, addr_symtab);

static vec_symtabs addr_symtabs = { 0 };

static void
__attribute__((constructor))
addr_init (void)
{
  bfd_init();
}

static int addr_comp(const void *p1, const void *p2)
{
  asymbol *const*s1 = p1;
  asymbol *const*s2 = p2;

  long a1 = (*s1)->value + bfd_get_section_vma((*s1)->section->owner, (*s1)->section);
  long a2 = (*s2)->value + bfd_get_section_vma((*s2)->section->owner, (*s2)->section);

  return a1 - a2;
}

static int
addr_extract_symbols_from_bfd (bfd *bfd)
{
  __precondition(ENOTSUP, 0 != bfd_check_format(bfd, bfd_object));

  long size;
  asymbol **symbols;
  long nsymbols;

  __checked_call(0 <= (size = bfd_get_symtab_upper_bound(bfd)));
  __checked_call(NULL != (symbols = malloc(size)));
  __checked_call(0 <= (nsymbols = bfd_canonicalize_symtab(bfd, symbols)),
    free(symbols);
  );

  qsort(symbols, nsymbols, sizeof(*symbols), &addr_comp);

  addr_symtab t = { 0 };

  long i;
  for (i = 0; i < nsymbols; ++i)
    {
      if (!(symbols[i]->flags & BSF_FUNCTION))
        continue;

      unsigned long a = symbols[i]->value;
      a += bfd_get_section_vma(bfd, symbols[i]->section);

      unsigned long b = symbols[i + 1]->value - 1;
      b += bfd_get_section_vma(bfd, symbols[i + 1]->section);

      if (!a)
        continue;

      if (!t.lower)
        t.lower = a;

      addr_symbol s = { a, b, symbols[i]->name };
      vector_push(&(t.symbols), s);
    }

  if (t.lower)
    t.upper = t.symbols.items[t.symbols.nitems - 1].upper;

  vector_push(&addr_symtabs, t);

  return 0;
}

int
addr_extract_symbols (const char *filename)
{
  bfd *bfd;
  __checked_call(NULL != (bfd = bfd_openr(filename, NULL)));

  __checked_call(0 == addr_extract_symbols_from_bfd(bfd),
    bfd_close(bfd);
  );

  return 0;
}

static addr_symbol*
addr_find_symbol (uintptr_t pc)
{
  size_t i;
  for (i = 0; i < addr_symtabs.nitems; ++i)
    if (addr_symtabs.items[i].lower <= pc && pc <= addr_symtabs.items[i].upper)
      {
        addr_symtab *s = addr_symtabs.items + i;
        size_t j;
        for (j = 0; j < s->symbols.nitems; ++j)
          if (s->symbols.items[j].lower <= pc && pc <= s->symbols.items[j].upper)
            return s->symbols.items + j;
      }

  return NULL;
}

int
addr_translate (uintptr_t pc, char **function, char **file, unsigned int *line)
{
  addr_symbol *b = addr_find_symbol(pc);

  if (function)
    __checked_call(NULL != (*function = strdup(b ? b->name : "??")));

  if (file)
    __checked_call(NULL != (*file = strdup("??")));

  if (line)
    *line = 0;

  return 0;
}

static void
__attribute__ ((destructor))
addr_fini ()
{

}

