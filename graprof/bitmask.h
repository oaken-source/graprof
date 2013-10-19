
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

#pragma once

/* this struct represents a simple bitmask of variable length
 */

struct bitmask
{
  unsigned int *fields;
  unsigned int nfields;
};

typedef struct bitmask bitmask;

/* return a pointer to a newly allocated and zero initialized bitmask
 * structure. Note that to avoid memory leakage, this structure has to be 
 * freed by using bitmask_destroy.
 *
 * params:
 *   width - the width of the bitmask in bits
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc
 *
 * returns:
 *   a pointer to a bitmask structure on success, NULL otherwise
 */
bitmask *bitmask_create(unsigned int width);

/* return a pointer to a newly allocated deep copy of a given bitmask. Note
 * that to avoid memory leakage, this structure has to be freed by using
 * bitmask_destroy.
 *
 * params:
 *   b - a pointer to a bitmask structure
 *
 * errors:
 *   may fail and set errno for the same reasons as malloc
 *
 * returns:
 *   a pointer to a bitmask structure on success, NULL otherwise
 */
bitmask *bitmask_copy(bitmask *b);

/* destroy a bitmask structure and free all associated memory
 *
 * params:
 *   b - a pointer to a pointer to a bitmask structure
 */
void bitmask_destroy(bitmask **b);

/* set a bit in a bitmask
 *
 * params:
 *   b - a pointer to a bitmask structure
 *   i - the index of the bit to set
 *
 * errors:
 *   the behaviour is undefined if i is greater than or equal to the width of
 *   the bitmask
 */
void bitmask_set(bitmask *b, unsigned int i);

/* get a bit in a bitmask
 *
 * params:
 *   b - a pointer to a bitmask structure
 *   i - the index of the bit to get
 *
 * errors:
 *   the behaviour is undefined if i is greater than or equal to the width of
 *   the bitmask
 *
 * returns:
 *   a positive integer if the specified bit is set, 0 otherwise
 */
int bitmask_get(bitmask *b, unsigned int i);
