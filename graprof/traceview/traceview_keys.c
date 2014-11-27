
/******************************************************************************
 *             graprof - a profiling and trace analysis tool                  *
 *                                                                            *
 *    Copyright (C) 2014  Andreas Grapentin                                   *
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


#include "traceview_keys.h"

#include <grapes/util.h>

#if HAVE_NCURSES

struct key_info
{
  char key;
  traceview_key value;

  size_t nchildren;
  struct key_info *children;
};
typedef struct key_info key_info;

key_info keys = { 0, TRACEVIEW_KEY_NONE, 0, NULL };

static key_info* __may_fail
push_key (key_info *i, char code, traceview_key k)
{
  ++(i->nchildren);
  i->children = realloc(i->children, sizeof(*(i->children)) * i->nchildren);
  assert_inner_ptr(i->children, "realloc");

  key_info *tmp = i->children + i->nchildren - 1;
  tmp->key = code;
  tmp->value = k;
  tmp->nchildren = 0;
  tmp->children = NULL;

  return tmp;
}

#define DEFINE_KEYS(GROUP) \
  key_info *current = &keys; \
  GROUP

#define KEY(CODE, KEY) \
  do \
    { \
      key_info *res = push_key(current, (CODE), (KEY)); \
      assert_inner(res != NULL, "push_key"); \
    } \
  while (0);

#define KEY_GROUP(CODE, GROUP) \
  do \
    { \
      key_info *tmp = push_key(current, (CODE), TRACEVIEW_KEY_NONE); \
      assert_inner(tmp != NULL, "push_key"); \
      key_info *current = tmp; \
      GROUP \
    } \
  while (0);

int
traceview_keys_init (void)
{

  DEFINE_KEYS(

    KEY( 'q', TRACEVIEW_KEY_QUIT)
    KEY( 'Q', TRACEVIEW_KEY_QUIT)
    KEY(0x0A, TRACEVIEW_KEY_ENTER)
    KEY(0x7F, TRACEVIEW_KEY_BACKSPACE)

    KEY_GROUP(0x1B,

      KEY( '1', TRACEVIEW_KEY_ALT_1)
      KEY( '2', TRACEVIEW_KEY_ALT_2)
      KEY( '3', TRACEVIEW_KEY_ALT_3)
      KEY( '4', TRACEVIEW_KEY_ALT_4)
      KEY( ERR, TRACEVIEW_KEY_ESCAPE)

      KEY_GROUP(0x5B,

        KEY(0x41, TRACEVIEW_KEY_ARROW_UP)
        KEY(0x42, TRACEVIEW_KEY_ARROW_DOWN)
        KEY(0x43, TRACEVIEW_KEY_ARROW_RIGHT)
        KEY(0x44, TRACEVIEW_KEY_ARROW_LEFT)

        KEY_GROUP(0x35, KEY(0x7E, TRACEVIEW_KEY_PAGE_UP))
        KEY_GROUP(0x36, KEY(0x7E, TRACEVIEW_KEY_PAGE_DOWN))

        KEY_GROUP(0x31,

          KEY_GROUP(0x31, KEY(0x7E, TRACEVIEW_KEY_F1))
          KEY_GROUP(0x32, KEY(0x7E, TRACEVIEW_KEY_F2))
          KEY_GROUP(0x33, KEY(0x7E, TRACEVIEW_KEY_F3))
          KEY_GROUP(0x34, KEY(0x7E, TRACEVIEW_KEY_F4))
          KEY_GROUP(0x35, KEY(0x7E, TRACEVIEW_KEY_F5))
          KEY_GROUP(0x37, KEY(0x7E, TRACEVIEW_KEY_F6))
          KEY_GROUP(0x38, KEY(0x7E, TRACEVIEW_KEY_F7))
          KEY_GROUP(0x39, KEY(0x7E, TRACEVIEW_KEY_F8))
        )

        KEY_GROUP(0x32,

          KEY_GROUP(0x30, KEY(0x7E, TRACEVIEW_KEY_F9))
          KEY_GROUP(0x31, KEY(0x7E, TRACEVIEW_KEY_F10))
          KEY_GROUP(0x33, KEY(0x7E, TRACEVIEW_KEY_F11))
          KEY_GROUP(0x34, KEY(0x7E, TRACEVIEW_KEY_F12))
        )
      )
    )
  )

  return 0;
}

#undef KEY_GROUP
#undef KEY
#undef DEFINE_KEYS

static void
traceview_keys_fini_walker (key_info *k)
{
  if (!(k->nchildren))
    return;

  size_t i;
  for (i = 0; i < k->nchildren; ++i)
    traceview_keys_fini_walker(k->children + i);

  free(k->children);
}

void
traceview_keys_fini (void)
{
  traceview_keys_fini_walker(&keys);
}

static char
wgetch_immediate(WINDOW *w)
{
  int res = nodelay(w, 1);
  assert_inner(res != ERR, "nodelay");

  char k = wgetch(w);

  res = nodelay(w, 0);
  assert_inner(res != ERR, "nodelay");

  return k;
}

static key_info* __may_fail
get_match (key_info *k, char c)
{
  size_t i;
  for (i = 0; i < k->nchildren; ++i)
    if (k->children[i].key == c)
      return k->children + i;

  return NULL;
}

traceview_key
traceview_keys_get (WINDOW *w)
{
  key_info *current = &keys;
  while (current->value == TRACEVIEW_KEY_NONE)
    {
      char k = (current == &keys) ? wgetch(w) : wgetch_immediate(w);
      current = get_match(current, k);

      if (!current)
        return TRACEVIEW_KEY_NONE;
    }

  return current->value;
}


#endif // HAVE_NCURSES
