#include "game.h"
#include "laby.h"
#include "minunit.h"
#include "rtmterm.c"
#include "u8.h"

static char *
empty_laby_test ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  level level = LEVEL_EMPTY;
  laby_init_empty (&level.lab, 1, 1);

  char *expected = "┏━━━┓\n"
                   "┃ @ ┃\n"
                   "┗━━━┛";
  // when:
  render_level (&level, &buf);
  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
simple_laby_test ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  level level = LEVEL_EMPTY;
  laby_init_empty (&level.lab, 3, 3);
  laby_add_border (&level.lab, 0, 1, (RIGHT_BORDER | BOTTOM_BORDER));
  laby_add_border (&level.lab, 1, 1, (LEFT_BORDER | BOTTOM_BORDER));
  laby_add_border (&level.lab, 2, 2, UPPER_BORDER);

  char *expected = "┏━━━━━━━┳━━━┓\n"
                   "┃ @     ┃   ┃\n"
                   "┃   ┏━━━┛   ┃\n"
                   "┃   ┃       ┃\n"
                   "┃   ┗━━━━━━━┫\n"
                   "┃           ┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  render_level (&level, &buf);

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
generate_eller_test ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  char *expected = "┏━━━━━━━━━━━━━━━┳━━━┓\n"
                   "┃ @             ┃   ┃\n"
                   "┃   ━━━━━━━━┳━━━┛   ┃\n"
                   "┃           ┃       ┃\n"
                   "┣━━━━━━━    ┗━━━    ┃\n"
                   "┃                   ┃\n"
                   "┗━━━━━━━━━━━━━━━━━━━┛";

  // when:
  level level = LEVEL_EMPTY;
  laby_generate (&level.lab, 3, 5, 1);
  render_level (&level, &buf);

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
  level level = LEVEL_EMPTY;
  level.player.visible_range = 2;
  laby_init_empty (&level.lab, 4, 4);
  mark_visible_rooms (&level);
  char *expected = "┏━━━━━━━━━━━━━━━┓\n"
                   "┃·@·········    ┃\n"
                   "┃···········    ┃\n"
                   "┃···········    ┃\n"
                   "┃···········    ┃\n"
                   "┃···········    ┃\n"
                   "┃               ┃\n"
                   "┃               ┃\n"
                   "┗━━━━━━━━━━━━━━━┛";

  // when:
  render_level (&level, &buf);

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
  level level = LEVEL_EMPTY;
  level.player.y = 1;
  level.player.x = 1;
  level.player.visible_range = 2;
  laby_init_empty (&level.lab, 3, 3);
  laby_add_border (&level.lab, 1, 1,
                   LEFT_BORDER | UPPER_BORDER | RIGHT_BORDER | BOTTOM_BORDER);
  mark_visible_rooms (&level);

  char *expected = "┏━━━━━━━━━━━┓\n"
                   "┃ ⛿         ┃\n"
                   "┃   ┏━━━┓   ┃\n"
                   "┃   ┃·@·┃   ┃\n"
                   "┃   ┗━━━┛   ┃\n"
                   "┃           ┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  render_level (&level, &buf);

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_3 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  level level = LEVEL_EMPTY;
  level.player.y = 1;
  level.player.x = 1;
  level.player.visible_range = 2;
  laby_init_empty (&level.lab, 3, 3);
  laby_add_border (&level.lab, 0, 0, RIGHT_BORDER | BOTTOM_BORDER);
  laby_add_border (&level.lab, 0, 2, LEFT_BORDER | BOTTOM_BORDER);
  laby_add_border (&level.lab, 2, 0, RIGHT_BORDER | UPPER_BORDER);
  laby_add_border (&level.lab, 2, 2, LEFT_BORDER | UPPER_BORDER);
  mark_visible_rooms (&level);

  char *expected = "┏━━━┳━━━┳━━━┓\n"
                   "┃ ⛿ ┃···┃   ┃\n"
                   "┣━━━┛···┗━━━┫\n"
                   "┃·····@·····┃\n"
                   "┣━━━┓···┏━━━┫\n"
                   "┃   ┃···┃   ┃\n"
                   "┗━━━┻━━━┻━━━┛";

  // when:
  render_level (&level, &buf);

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_4 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  level level = LEVEL_EMPTY;
  level.player.y = 1;
  level.player.x = 1;
  level.player.visible_range = 2;
  laby_init_empty (&level.lab, 3, 3);
  laby_add_border (&level.lab, 0, 0, RIGHT_BORDER);
  laby_add_border (&level.lab, 1, 1, LEFT_BORDER | RIGHT_BORDER);
  laby_add_border (&level.lab, 2, 2, LEFT_BORDER);
  mark_visible_rooms (&level);

  char *expected = "┏━━━┳━━━━━━━┓\n"
                   "┃ ⛿ ┃·······┃\n"
                   "┃   ┃···┃   ┃\n"
                   "┃   ┃·@·┃   ┃\n"
                   "┃·······┃   ┃\n"
                   "┃·······┃   ┃\n"
                   "┗━━━━━━━┻━━━┛";

  // when:
  render_level (&level, &buf);

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_5 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  level level = LEVEL_EMPTY;
  level.player.y = 1;
  level.player.x = 0;
  level.player.visible_range = 2;
  laby_init_empty (&level.lab, 3, 3);
  laby_add_border (&level.lab, 0, 0, BOTTOM_BORDER);
  laby_add_border (&level.lab, 1, 1, UPPER_BORDER | BOTTOM_BORDER);
  laby_add_border (&level.lab, 2, 2, UPPER_BORDER);
  mark_visible_rooms (&level);

  char *expected = "┏━━━━━━━━━━━┓\n"
                   "┃ ⛿         ┃\n"
                   "┣━━━━━━━····┃\n"
                   "┃·@·········┃\n"
                   "┃···━━━━━━━━┫\n"
                   "┃·······    ┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  render_level (&level, &buf);

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_6 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  level level = LEVEL_EMPTY;
  level.player.y = 1;
  level.player.x = 1;
  level.player.visible_range = 1;
  laby_init_empty (&level.lab, 3, 3);
  laby_add_border (&level.lab, 0, 1, BOTTOM_BORDER);
  laby_add_border (&level.lab, 2, 2, UPPER_BORDER);
  mark_visible_rooms (&level);

  char *expected = "┏━━━━━━━━━━━┓\n"
                   "┃·⛿·    ····┃\n"
                   "┃···━━━━····┃\n"
                   "┃·····@·····┃\n"
                   "┃·······━━━━┫\n"
                   "┃···········┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  render_level (&level, &buf);

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_7 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  level level = LEVEL_EMPTY;
  level.player.visible_range = 2;
  laby_init_empty (&level.lab, 3, 3);
  laby_add_border (&level.lab, 0, 1, RIGHT_BORDER);
  laby_add_border (&level.lab, 1, 1, RIGHT_BORDER);
  mark_visible_rooms (&level);

  char *expected = "┏━━━━━━━┳━━━┓\n"
                   "┃·@·····┃   ┃\n"
                   "┃·······┃   ┃\n"
                   "┃·······┃   ┃\n"
                   "┃···········┃\n"
                   "┃···········┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  render_level (&level, &buf);

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

char *
laby_visibility_test_8 ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  level level = LEVEL_EMPTY;
  level.player.visible_range = 2;
  level.player.y = 2;
  level.player.x = 2;
  laby_init_empty (&level.lab, 3, 3);
  laby_add_border (&level.lab, 0, 0, BOTTOM_BORDER);
  laby_add_border (&level.lab, 0, 1, RIGHT_BORDER);
  laby_add_border (&level.lab, 1, 2, LEFT_BORDER | BOTTOM_BORDER);
  mark_visible_rooms (&level);

  char *expected = "┏━━━━━━━┳━━━┓\n"
                   "┃·⛿··   ┃   ┃\n"
                   "┣━━━····┃   ┃\n"
                   "┃·······┃   ┃\n"
                   "┃·······┗━━━┫\n"
                   "┃·········@·┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  render_level (&level, &buf);

  // then:
  u8str actual = u8_buffer_to_u8str (&buf);
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}
