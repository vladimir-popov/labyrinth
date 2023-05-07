#include "minunit.h"
#include "u8.h"
#include "game.h"
#include "rtmterm.c"

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
  u8str actual = u8_buffer_to_dstr (&buf);
  mu_dstr_eq_to_str (actual, expected);
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

  laby_mark_as_visited (&level.lab, 0, 0);
  laby_mark_as_visited (&level.lab, 0, 1);
  laby_mark_as_visited (&level.lab, 1, 0);

  char *expected = "┏━━━━━━━┳━━━┓\n"
                   "┃·@·····┃   ┃\n"
                   "┃···┏━━━┛   ┃\n"
                   "┃···┃       ┃\n"
                   "┃···┗━━━━━━━┫\n"
                   "┃           ┃\n"
                   "┗━━━━━━━━━━━┛";

  // when:
  render_level (&level, &buf);

  // then:
  u8str actual = u8_buffer_to_dstr (&buf);
  mu_dstr_eq_to_str (actual, expected);
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
  u8str actual = u8_buffer_to_dstr (&buf);
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
rtm_all_tests ()
{
  mu_run_test (empty_laby_test);
  mu_run_test (simple_laby_test);
  mu_run_test (generate_eller_test);
  return 0;
}
