#include "laby.h"
#include "minunit.h"
#include "render.h"
#include "u8.h"

int screen_rows = 0;
int screen_cols = 0;

/* The count of symbols of one room.  */
const int laby_room_height = 2;
const int laby_room_width = 4;

/* The count of visible chars. */
const int game_window_rows = 25;
const int game_window_cols = 78;

/*
 * This macros must follow after the code:
 * ```
 * smap sm;
 * u8buf buf = U8_BUF_EMPTY;
 * Laby lab;
 * ```
 */
#define RENDER(DRAW)                                                          \
  do                                                                          \
    {                                                                         \
      smap_init (&sm, lab.rows, lab.cols, laby_room_height, laby_room_width); \
      DRAW;                                                                   \
      render_symbols_map (&buf, &sm);                                         \
      smap_free (&sm);                                                        \
    }                                                                         \
  while (0)

static char *
empty_laby_test ()
{
  // given:
  smap sm;
  u8buf buf = U8_BUF_EMPTY;
  Laby lab;
  laby_init_empty (&lab, 1, 1);

  char *expected = "┏━━━┓\n"
                   "┃   ┃\n"
                   "┗━━━┛";
  // when:
  RENDER (draw_laby (&sm, &lab));
  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
simple_laby_test ()
{
  // given:
  smap sm;
  u8buf buf = U8_BUF_EMPTY;
  Laby lab;
  laby_init_empty (&lab, 3, 3);
  laby_add_border (&lab, 0, 1, (RIGHT_BORDER | BOTTOM_BORDER));
  laby_add_border (&lab, 1, 1, (LEFT_BORDER | BOTTOM_BORDER));
  laby_add_border (&lab, 2, 2, UPPER_BORDER);

  char *expected = "┏━━━━━━━┳━━━┓\n"
                   "┃       ┃   ┃\n"
                   "┃   ┏━━━┛   ┃\n"
                   "┃   ┃       ┃\n"
                   "┃   ┗━━━━━━━┫\n"
                   "┃           ┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  RENDER (draw_laby (&sm, &lab));

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
generate_eller_test ()
{
  // given:
  smap sm;
  u8buf buf = U8_BUF_EMPTY;
  char *expected = "┏━━━━━━━━━━━━━━━┳━━━┓\n"
                   "┃               ┃   ┃\n"
                   "┃   ━━━━━━━━┳━━━┛   ┃\n"
                   "┃           ┃       ┃\n"
                   "┣━━━━━━━    ┗━━━    ┃\n"
                   "┃                   ┃\n"
                   "┗━━━━━━━━━━━━━━━━━━━┛";

  // when:
  Laby lab;
  laby_generate (&lab, 3, 5, 1);
  RENDER (draw_laby (&sm, &lab));

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
visibility_in_open_space_test ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  smap sm;
  Laby lab;
  int r = 2;
  int c = 2;
  int range = 1;
  laby_init_empty (&lab, 5, 5);
  laby_mark_visible_rooms (&lab, r, c, range);
  char *expected = "┏━━━━━━━━━━━━━━━━━━━┓\n"
                   "┃                   ┃\n"
                   "┃   ············    ┃\n"
                   "┃   ············    ┃\n"
                   "┃   ············    ┃\n"
                   "┃   ······@·····    ┃\n"
                   "┃   ············    ┃\n"
                   "┃   ············    ┃\n"
                   "┃   ············    ┃\n"
                   "┃                   ┃\n"
                   "┗━━━━━━━━━━━━━━━━━━━┛";

  // when:
  RENDER ({
    draw_laby (&sm, &lab);
    draw_in_the_middle_of_room (&sm, r, c, SIDX_PLAYER);
  });

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
visibility_in_closed_space_test_1 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  smap sm;
  Laby lab;
  int r = 0;
  int c = 0;
  int range = 3;
  laby_init_empty (&lab, 1, 1);
  laby_mark_visible_rooms (&lab, r, c, range);
  char *expected = "┏━━━┓\n"
                   "┃·@·┃\n"
                   "┗━━━┛";

  // when:
  RENDER ({
    draw_laby (&sm, &lab);
    draw_in_the_middle_of_room (&sm, r, c, SIDX_PLAYER);
  });

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
visibility_in_closed_space_test_2 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  smap sm;
  Laby lab;
  int r = 2;
  int c = 2;
  int range = 3;
  laby_init_empty (&lab, 5, 5);
  laby_add_border (&lab, 1, 1, LEFT_BORDER | UPPER_BORDER);
  laby_add_border (&lab, 1, 2, UPPER_BORDER);
  laby_add_border (&lab, 1, 3, RIGHT_BORDER | UPPER_BORDER);
  laby_add_border (&lab, 2, 1, LEFT_BORDER);
  laby_add_border (&lab, 2, 3, RIGHT_BORDER);
  laby_add_border (&lab, 3, 1, LEFT_BORDER | BOTTOM_BORDER);
  laby_add_border (&lab, 3, 2, BOTTOM_BORDER);
  laby_add_border (&lab, 3, 3, RIGHT_BORDER | BOTTOM_BORDER);
  laby_mark_visible_rooms (&lab, r, c, range);
  char *expected = "┏━━━━━━━━━━━━━━━━━━━┓\n"
                   "┃                   ┃\n"
                   "┃   ┏━━━━━━━━━━━┓   ┃\n"
                   "┃   ┃···········┃   ┃\n"
                   "┃   ┃···········┃   ┃\n"
                   "┃   ┃·····@·····┃   ┃\n"
                   "┃   ┃···········┃   ┃\n"
                   "┃   ┃···········┃   ┃\n"
                   "┃   ┗━━━━━━━━━━━┛   ┃\n"
                   "┃                   ┃\n"
                   "┗━━━━━━━━━━━━━━━━━━━┛";

  // when:
  RENDER ({
    draw_laby (&sm, &lab);
    draw_in_the_middle_of_room (&sm, r, c, SIDX_PLAYER);
  });

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_crossroads_test ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  smap sm;
  Laby lab;
  int r = 1;
  int c = 1;
  int range = 3;
  laby_init_empty (&lab, 3, 3);
  laby_add_border (&lab, 0, 0, RIGHT_BORDER | BOTTOM_BORDER);
  laby_add_border (&lab, 0, 2, LEFT_BORDER | BOTTOM_BORDER);
  laby_add_border (&lab, 2, 0, RIGHT_BORDER | UPPER_BORDER);
  laby_add_border (&lab, 2, 2, LEFT_BORDER | UPPER_BORDER);
  laby_mark_visible_rooms (&lab, r, c, range);

  char *expected = "┏━━━┳━━━┳━━━┓\n"
                   "┃   ┃···┃   ┃\n"
                   "┣━━━┛···┗━━━┫\n"
                   "┃·····@·····┃\n"
                   "┣━━━┓···┏━━━┫\n"
                   "┃   ┃···┃   ┃\n"
                   "┗━━━┻━━━┻━━━┛";

  // when:
  RENDER ({
    draw_laby (&sm, &lab);
    draw_in_the_middle_of_room (&sm, r, c, SIDX_PLAYER);
  });

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_1 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  smap sm;
  Laby lab;
  int r = 1;
  int c = 1;
  int range = 3;
  laby_init_empty (&lab, 3, 3);
  laby_add_border (&lab, 0, 0, RIGHT_BORDER);
  laby_add_border (&lab, 1, 1, LEFT_BORDER | RIGHT_BORDER);
  laby_add_border (&lab, 2, 2, LEFT_BORDER);
  laby_mark_visible_rooms (&lab, r, c, range);

  char *expected = "┏━━━┳━━━━━━━┓\n"
                   "┃   ┃···    ┃\n"
                   "┃   ┃···┃   ┃\n"
                   "┃   ┃·@·┃   ┃\n"
                   "┃   ····┃   ┃\n"
                   "┃   ····┃   ┃\n"
                   "┗━━━━━━━┻━━━┛";

  // when:
  RENDER ({
    draw_laby (&sm, &lab);
    draw_in_the_middle_of_room (&sm, r, c, SIDX_PLAYER);
  });

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_2 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  smap sm;
  Laby lab;
  int r = 1;
  int c = 1;
  int range = 3;
  laby_init_empty (&lab, 3, 3);
  laby_add_border (&lab, 0, 1, BOTTOM_BORDER);
  laby_add_border (&lab, 2, 2, UPPER_BORDER);
  laby_mark_visible_rooms (&lab, r, c, range);

  char *expected = "┏━━━━━━━━━━━┓\n"
                   "┃           ┃\n"
                   "┃···━━━━····┃\n"
                   "┃·····@·····┃\n"
                   "┃·······━━━━┫\n"
                   "┃·······    ┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  RENDER ({
    draw_laby (&sm, &lab);
    draw_in_the_middle_of_room (&sm, r, c, SIDX_PLAYER);
  });

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}
