#include "game.h"
#include "laby.h"
#include "minunit.h"
#include "render.h"
#include "u8.h"

int terminal_window_height = 0;
int terminal_window_width = 0;

/* The count of symbols of one room.  */
const int laby_room_height = 2;
const int laby_room_width = 4;

/* The count of visible chars. */
const int game_window_height = 25;
const int game_window_width = 78;

/* util functions */
static void
init_known_empty (Laby *lab, int rows, int cols)
{
  laby_init_empty (lab, rows, cols);
  laby_mark_whole_as_known (lab);
}
/* ------------------------------ */

static char *
empty_laby_test ()
{
  // given:
  Render render = DEFAULT_RENDER;
  Laby lab;
  init_known_empty (&lab, 1, 1);

  char *expected = "┏━━━┓\n"
                   "┃   ┃\n"
                   "┗━━━┛";
  // when:
  render_laby (&render, &lab);
  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
simple_laby_test ()
{
  // given:
  Render render = DEFAULT_RENDER;
  Laby lab;
  init_known_empty (&lab, 3, 3);
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
  render_laby (&render, &lab);

  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
generate_eller_test ()
{
  // given:
  Render render = DEFAULT_RENDER;
  char *expected = "┏━━━┳━━━━━━━┳━━━━━━━┓\n"
                   "┃   ┃       ┃       ┃\n"
                   "┃       ┃   ┗━━━┓   ┃\n"
                   "┃       ┃       ┃   ┃\n"
                   "┣━━━    ┗━━━┓       ┃\n"
                   "┃           ┃       ┃\n"
                   "┣━━━━━━━┓       ┏━━━┫\n"
                   "┃       ┃       ┃   ┃\n"
                   "┣━━━    ┗━━━        ┃\n"
                   "┃                   ┃\n"
                   "┗━━━━━━━━━━━━━━━━━━━┛";

  // when:
  Laby lab;
  laby_generate (&lab, 5, 5);

  // then:
  laby_mark_whole_as_known (&lab);
  render_laby (&render, &lab);
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
render_laby_with_player_test ()
{
  // given:
  int laby_rows = 7;
  int laby_cols = 7;
  int visible_rows = 3;
  int visible_cols = 3;
  Render render = render_create (2, 4, visible_rows * 2, visible_cols * 4);
  Player player = { 3, 3, 2 };
  Laby lab;
  laby_generate (&lab, laby_rows, laby_cols);
  laby_set_content (&lab, player.row, player.col, C_PLAYER);
  laby_mark_whole_as_known (&lab);
  // The whole laby:
  // ┏━━━━━━━┳━━━━━━━┳━━━━━━━━━━━┓
  // ┃       ┃       ┃           ┃
  // ┣━━━┓   ┃   ━━━━┛   ┏━━━━━━━┫
  // ┃   ┃   ┃           ┃       ┃
  // ┃           ━━━━┳━━━╋━━━    ┃
  // ┃               ┃   ┃       ┃
  // ┣━━━━━━━┳━━━┓       ┗━━━    ┃
  // ┃       ┃   ┃ @             ┃
  // ┣━━━        ┗━━━┳━━━━━━━    ┃
  // ┃               ┃           ┃
  // ┣━━━┳━━━┓   ━━━━╋━━━┳━━━    ┃
  // ┃   ┃   ┃       ┃   ┃       ┃
  // ┃       ┗━━━        ┗━━━    ┃
  // ┃                           ┃
  // ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
  char *expected = "    ━━━━┳━━━╋\n"
                   "        ┃   ┃\n"
                   "┳━━━┓       ┗\n"
                   "┃   ┃ @      \n"
                   "    ┗━━━┳━━━━\n"
                   "        ┃    \n"
                   "┓   ━━━━╋━━━┳";

  // when:
  render_update_visible_area (&render, &player, laby_rows, laby_cols);
  render_laby (&render, &lab);

  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
visibility_in_open_space_test ()
{
  // given:
  Render render = DEFAULT_RENDER;
  Laby lab;
  int r = 2;
  int c = 2;
  int range = 1;
  init_known_empty (&lab, 5, 5);
  laby_mark_visible_rooms (&lab, r, c, range);
  laby_set_content (&lab, r, c, C_PLAYER);
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
  render_laby (&render, &lab);

  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
visibility_in_closed_space_test_1 ()
{
  // given:
  Render render = DEFAULT_RENDER;
  Laby lab;
  int r = 0;
  int c = 0;
  int range = 3;
  init_known_empty (&lab, 1, 1);
  laby_mark_visible_rooms (&lab, r, c, range);
  laby_set_content (&lab, r, c, C_PLAYER);
  char *expected = "┏━━━┓\n"
                   "┃·@·┃\n"
                   "┗━━━┛";

  // when:
  render_laby (&render, &lab);

  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
visibility_in_closed_space_test_2 ()
{
  // given:
  Render render = DEFAULT_RENDER;
  Laby lab;
  int r = 2;
  int c = 2;
  int range = 3;
  init_known_empty (&lab, 5, 5);
  laby_add_border (&lab, 1, 1, LEFT_BORDER | UPPER_BORDER);
  laby_add_border (&lab, 1, 2, UPPER_BORDER);
  laby_add_border (&lab, 1, 3, RIGHT_BORDER | UPPER_BORDER);
  laby_add_border (&lab, 2, 1, LEFT_BORDER);
  laby_add_border (&lab, 2, 3, RIGHT_BORDER);
  laby_add_border (&lab, 3, 1, LEFT_BORDER | BOTTOM_BORDER);
  laby_add_border (&lab, 3, 2, BOTTOM_BORDER);
  laby_add_border (&lab, 3, 3, RIGHT_BORDER | BOTTOM_BORDER);
  laby_mark_visible_rooms (&lab, r, c, range);
  laby_set_content (&lab, r, c, C_PLAYER);
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
  render_laby (&render, &lab);

  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_crossroads_test ()
{
  // given:
  Render render = DEFAULT_RENDER;
  Laby lab;
  int r = 1;
  int c = 1;
  int range = 3;
  init_known_empty (&lab, 3, 3);
  laby_add_border (&lab, 0, 0, RIGHT_BORDER | BOTTOM_BORDER);
  laby_add_border (&lab, 0, 2, LEFT_BORDER | BOTTOM_BORDER);
  laby_add_border (&lab, 2, 0, RIGHT_BORDER | UPPER_BORDER);
  laby_add_border (&lab, 2, 2, LEFT_BORDER | UPPER_BORDER);
  laby_mark_visible_rooms (&lab, r, c, range);
  laby_set_content (&lab, r, c, C_PLAYER);

  char *expected = "┏━━━┳━━━┳━━━┓\n"
                   "┃   ┃···┃   ┃\n"
                   "┣━━━┛···┗━━━┫\n"
                   "┃·····@·····┃\n"
                   "┣━━━┓···┏━━━┫\n"
                   "┃   ┃···┃   ┃\n"
                   "┗━━━┻━━━┻━━━┛";

  // when:
  render_laby (&render, &lab);

  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_1 ()
{
  // given:
  Render render = DEFAULT_RENDER;
  Laby lab;
  int r = 1;
  int c = 1;
  int range = 3;
  init_known_empty (&lab, 3, 3);
  laby_add_border (&lab, 0, 0, RIGHT_BORDER);
  laby_add_border (&lab, 1, 1, LEFT_BORDER | RIGHT_BORDER);
  laby_add_border (&lab, 2, 2, LEFT_BORDER);
  laby_mark_visible_rooms (&lab, r, c, range);
  laby_set_content (&lab, r, c, C_PLAYER);

  char *expected = "┏━━━┳━━━━━━━┓\n"
                   "┃   ┃·······┃\n"
                   "┃   ┃···┃···┃\n"
                   "┃   ┃·@·┃   ┃\n"
                   "┃·······┃   ┃\n"
                   "┃·······┃   ┃\n"
                   "┗━━━━━━━┻━━━┛";

  // when:
  render_laby (&render, &lab);

  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}
char *
laby_visibility_test_2 ()
{
  // given:
  Render render = DEFAULT_RENDER;
  Laby lab;
  int r = 0;
  int c = 1;
  int range = 3;
  init_known_empty (&lab, 2, 3);
  laby_add_border (&lab, 0, 0, BOTTOM_BORDER);
  laby_add_border (&lab, 0, 2, BOTTOM_BORDER);
  laby_mark_visible_rooms (&lab, r, c, range);
  laby_set_content (&lab, r, c, C_PLAYER);

  char *expected = "┏━━━━━━━━━━━┓\n"
                   "┃·····@·····┃\n"
                   "┣━━━····━━━━┫\n"
                   "┃···········┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  render_laby (&render, &lab);

  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_3 ()
{
  // given:
  Render render = DEFAULT_RENDER;
  Laby lab;
  int r = 0;
  int c = 1;
  int range = 3;
  init_known_empty (&lab, 3, 3);
  laby_add_border (&lab, 0, 0, BOTTOM_BORDER);
  laby_add_border (&lab, 0, 2, BOTTOM_BORDER);
  laby_mark_visible_rooms (&lab, r, c, range);
  laby_set_content (&lab, r, c, C_PLAYER);

  char *expected = "┏━━━━━━━━━━━┓\n"
                   "┃·····@·····┃\n"
                   "┣━━━····━━━━┫\n"
                   "┃···········┃\n"
                   "┃···········┃\n"
                   "┃···········┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  render_laby (&render, &lab);

  // then:
  u8str actual = u8_buffer_to_u8str (&render.buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}
