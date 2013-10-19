
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

#include "bitmask.h"

#include <string.h>
#include <stdlib.h>

#include <grapes/util.h>

#define FIELDWIDTH (8 * sizeof(unsigned int))

bitmask*
bitmask_create (unsigned int width)
{
  bitmask *b = malloc(sizeof(bitmask));
  assert_inner_ptr(b, "malloc");

  b->nfields = width / FIELDWIDTH;
  if (width % FIELDWIDTH)
    ++(b->nfields);

  b->fields = malloc(sizeof(unsigned int) * b->nfields);
  assert_inner_ptr(b->fields, "malloc");

  return b;
}

bitmask*
bitmask_copy (bitmask *b)
{
  bitmask *c = malloc(sizeof(bitmask));
  assert_inner_ptr(c);

  c->nfields = b->nfields;
  c->fields = malloc(sizeof(unsigned int) * c->nfields);
  assert_inner_ptr(c->fields, "malloc");

  memcpy(c->fields, b->fields, c->nfields * sizeof(unsigned int));

  return c;
}

void 
bitmask_destroy (bitmask **b)
{
  free((*b)->fields);
  free(*b);
}

void
bitmask_set (bitmask *b, unsigned int i)
{
  b->fields[i / FIELDWIDTH] |= ((unsigned int)1 << (i % FIELDWIDTH));
}

int
bitmask_get (bitmask *b, unsigned int i)
{
  return b->fields[i / FIELDWIDTH] & ((unsigned int)1 << (i % FIELDWIDTH));
}
