
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


#include "bitmask.h"

#include <string.h>
#include <stdlib.h>

#include <grapes/util.h>

#define FIELDWIDTH (8 * sizeof(unsigned int))

bitmask*
bitmask_create (unsigned int width)
{
  __returns_ptr;

  bitmask *b;

  __checked_call(NULL != (b = malloc(sizeof(bitmask))));

  b->width = width;
  b->nfields = width / FIELDWIDTH;
  if (width % FIELDWIDTH)
    ++(b->nfields);

  __checked_call(NULL != (b->fields = calloc(b->nfields, sizeof(unsigned int))));

  return b;
}

bitmask*
bitmask_copy (bitmask *b)
{
  __returns_ptr;

  bitmask *c;

  __checked_call(NULL != (c = malloc(sizeof(bitmask))));

  c->width = b->width;
  c->nfields = b->nfields;
  __checked_call(NULL != (c->fields = malloc(sizeof(unsigned int) * c->nfields)));

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
