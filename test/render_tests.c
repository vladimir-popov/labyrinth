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

static void
render_laby (Laby *lab, U8_Buf *buf)
{
  smap sm;
  symbols_map_init (&sm, lab->rows, lab->cols, laby_room_height,
                    laby_room_width);
  draw_laby (&sm, lab);
  render_symbols_map (buf, &sm);
  symbols_map_free (&sm);
}

static char *
empty_laby_test ()
{
  // given:
  U8_Buf buf = U8_BUF_EMPTY;
  Laby lab;
  laby_init_empty (&lab, 1, 1);

  char *expected = "┏━━━┓\n"
                   "┃   ┃\n"
                   "┗━━━┛";
  // when:
  render_laby (&lab, &buf);
  // then:
  U8_Str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
simple_laby_test ()
{
  // given:
  U8_Buf buf = U8_BUF_EMPTY;
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
  render_laby (&lab, &buf);

  // then:
  U8_Str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
generate_eller_test ()
{
  // given:
  U8_Buf buf = U8_BUF_EMPTY;
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
  render_laby (&lab, &buf);

  // then:
  U8_Str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}
//
// char *
// laby_visibility_test_1 ()
// {
//   // given:
//   U8_Buf buf = U8_BUF_EMPTY;
//   Laby lab;
//   laby_init_empty (&lab, 4, 4);
//   char *expected = "┏━━━━━━━━━━━━━━━┓\n"
//                    "┃·@·········    ┃\n"
//                    "┃···········    ┃\n"
//                    "┃···········    ┃\n"
//                    "┃···········    ┃\n"
//                    "┃···········    ┃\n"
//                    "┃               ┃\n"
//                    "┃               ┃\n"
//                    "┗━━━━━━━━━━━━━━━┛";
//
//   // when:
//   draw_laby (&level, &buf);
//
//   // then:
//   U8_Str actual = u8_buffer_to_u8str (&buf);
//   mu_u8str_eq_to_str (actual, expected);
//   return 0;
// }
//
// char *
// laby_visibility_test_2 ()
// {
//   // given:
//   U8_Buf buf = U8_BUF_EMPTY;
//   level level = LEVEL_EMPTY;
//   level.player.y = 1;
//   level.player.x = 1;
//   level.player.visible_range = 2;
//   laby_init_empty (&level.lab, 3, 3);
//   laby_add_border (&level.lab, 1, 1,
//                    LEFT_BORDER | UPPER_BORDER | RIGHT_BORDER |
//                    BOTTOM_BORDER);
//
//   char *expected = "┏━━━━━━━━━━━┓\n"
//                    "┃ ⛿         ┃\n"
//                    "┃   ┏━━━┓   ┃\n"
//                    "┃   ┃·@·┃   ┃\n"
//                    "┃   ┗━━━┛   ┃\n"
//                    "┃           ┃\n"
//                    "┗━━━━━━━━━━━┛";
//
//   // when:
//   render_level (&level, &buf);
//
//   // then:
//   U8_Str actual = u8_buffer_to_u8str (&buf);
//   mu_u8str_eq_to_str (actual, expected);
//   return 0;
// }
//
// char *
// laby_visibility_test_3 ()
// {
//   // given:
//   U8_Buf buf = U8_BUF_EMPTY;
//   level level = LEVEL_EMPTY;
//   level.player.y = 1;
//   level.player.x = 1;
//   level.player.visible_range = 2;
//   laby_init_empty (&level.lab, 3, 3);
//   laby_add_border (&level.lab, 0, 0, RIGHT_BORDER | BOTTOM_BORDER);
//   laby_add_border (&level.lab, 0, 2, LEFT_BORDER | BOTTOM_BORDER);
//   laby_add_border (&level.lab, 2, 0, RIGHT_BORDER | UPPER_BORDER);
//   laby_add_border (&level.lab, 2, 2, LEFT_BORDER | UPPER_BORDER);
//
//   char *expected = "┏━━━┳━━━┳━━━┓\n"
//                    "┃ ⛿ ┃···┃   ┃\n"
//                    "┣━━━┛···┗━━━┫\n"
//                    "┃·····@·····┃\n"
//                    "┣━━━┓···┏━━━┫\n"
//                    "┃   ┃···┃   ┃\n"
//                    "┗━━━┻━━━┻━━━┛";
//
//   // when:
//   render_level (&level, &buf);
//
//   // then:
//   U8_Str actual = u8_buffer_to_u8str (&buf);
//   mu_u8str_eq_to_str (actual, expected);
//   return 0;
// }
//
// char *
// laby_visibility_test_4 ()
// {
//   // given:
//   U8_Buf buf = U8_BUF_EMPTY;
//   level level = LEVEL_EMPTY;
//   level.player.y = 1;
//   level.player.x = 1;
//   level.player.visible_range = 2;
//   laby_init_empty (&level.lab, 3, 3);
//   laby_add_border (&level.lab, 0, 0, RIGHT_BORDER);
//   laby_add_border (&level.lab, 1, 1, LEFT_BORDER | RIGHT_BORDER);
//   laby_add_border (&level.lab, 2, 2, LEFT_BORDER);
//
//   char *expected = "┏━━━┳━━━━━━━┓\n"
//                    "┃ ⛿ ┃·······┃\n"
//                    "┃   ┃···┃   ┃\n"
//                    "┃   ┃·@·┃   ┃\n"
//                    "┃·······┃   ┃\n"
//                    "┃·······┃   ┃\n"
//                    "┗━━━━━━━┻━━━┛";
//
//   // when:
//   render_level (&level, &buf);
//
//   // then:
//   U8_Str actual = u8_buffer_to_u8str (&buf);
//   mu_u8str_eq_to_str (actual, expected);
//   return 0;
// }
//
// char *
// laby_visibility_test_5 ()
// {
//   // given:
//   U8_Buf buf = U8_BUF_EMPTY;
//   level level = LEVEL_EMPTY;
//   level.player.y = 1;
//   level.player.x = 0;
//   level.player.visible_range = 2;
//   laby_init_empty (&level.lab, 3, 3);
//   laby_add_border (&level.lab, 0, 0, BOTTOM_BORDER);
//   laby_add_border (&level.lab, 1, 1, UPPER_BORDER | BOTTOM_BORDER);
//   laby_add_border (&level.lab, 2, 2, UPPER_BORDER);
//
//   char *expected = "┏━━━━━━━━━━━┓\n"
//                    "┃ ⛿         ┃\n"
//                    "┣━━━━━━━····┃\n"
//                    "┃·@·········┃\n"
//                    "┃···━━━━━━━━┫\n"
//                    "┃·······    ┃\n"
//                    "┗━━━━━━━━━━━┛";
//
//   // when:
//   render_level (&level, &buf);
//
//   // then:
//   U8_Str actual = u8_buffer_to_u8str (&buf);
//   mu_u8str_eq_to_str (actual, expected);
//   return 0;
// }
//
// char *
// laby_visibility_test_6 ()
// {
//   // given:
//   U8_Buf buf = U8_BUF_EMPTY;
//   level level = LEVEL_EMPTY;
//   level.player.y = 1;
//   level.player.x = 1;
//   level.player.visible_range = 1;
//   laby_init_empty (&level.lab, 3, 3);
//   laby_add_border (&level.lab, 0, 1, BOTTOM_BORDER);
//   laby_add_border (&level.lab, 2, 2, UPPER_BORDER);
//
//   char *expected = "┏━━━━━━━━━━━┓\n"
//                    "┃·⛿·    ····┃\n"
//                    "┃···━━━━····┃\n"
//                    "┃·····@·····┃\n"
//                    "┃·······━━━━┫\n"
//                    "┃···········┃\n"
//                    "┗━━━━━━━━━━━┛";
//
//   // when:
//   render_level (&level, &buf);
//
//   // then:
//   U8_Str actual = u8_buffer_to_u8str (&buf);
//   mu_u8str_eq_to_str (actual, expected);
//   return 0;
// }
//
// char *
// laby_visibility_test_7 ()
// {
//   // given:
//   U8_Buf buf = U8_BUF_EMPTY;
//   level level = LEVEL_EMPTY;
//   level.player.visible_range = 2;
//   laby_init_empty (&level.lab, 3, 3);
//   laby_add_border (&level.lab, 0, 1, RIGHT_BORDER);
//   laby_add_border (&level.lab, 1, 1, RIGHT_BORDER);
//
//   char *expected = "┏━━━━━━━┳━━━┓\n"
//                    "┃·@·····┃   ┃\n"
//                    "┃·······┃   ┃\n"
//                    "┃·······┃   ┃\n"
//                    "┃···········┃\n"
//                    "┃···········┃\n"
//                    "┗━━━━━━━━━━━┛";
//
//   // when:
//   render_level (&level, &buf);
//
//   // then:
//   U8_Str actual = u8_buffer_to_u8str (&buf);
//   mu_u8str_eq_to_str (actual, expected);
//   return 0;
// }
//
// char *
// laby_visibility_test_8 ()
// {
//   // given:
//   U8_Buf buf = U8_BUF_EMPTY;
//   level level = LEVEL_EMPTY;
//   level.player.visible_range = 2;
//   level.player.y = 2;
//   level.player.x = 2;
//   laby_init_empty (&level.lab, 3, 3);
//   laby_add_border (&level.lab, 0, 0, BOTTOM_BORDER);
//   laby_add_border (&level.lab, 0, 1, RIGHT_BORDER);
//   laby_add_border (&level.lab, 1, 2, LEFT_BORDER | BOTTOM_BORDER);
//
//   char *expected = "┏━━━━━━━┳━━━┓\n"
//                    "┃·⛿··   ┃   ┃\n"
//                    "┣━━━····┃   ┃\n"
//                    "┃·······┃   ┃\n"
//                    "┃·······┗━━━┫\n"
//                    "┃·········@·┃\n"
//                    "┗━━━━━━━━━━━┛";
//
//   // when:
//   render_level (&level, &buf);
//
//   // then:
//   U8_Str actual = u8_buffer_to_u8str (&buf);
//   mu_u8str_eq_to_str (actual, expected);
//   return 0;
// }
