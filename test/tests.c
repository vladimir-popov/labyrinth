#include <stdio.h>

#include "dbuf.h"
#include "game.h"
#include "minunit.h"
#include "rtmterm.c"
#include "term.c"

int tests_run = 0;

static char *
parse_string_to_buffer_test ()
{
  // given:
  char *template = "This is\n"
                   "a template";
  // when:
  dbuf buf;
  buffer_parse (&buf, template);
  dstr res = buffer_to_dstr (&buf);

  // then:
  mu_dstr_eq_to_str (res, template);
  return 0;
}

static char *
merge_buffers_test ()
{
  // given:
  dbuf first;
  buffer_parse (&first, ".........\n"
                        ".........\n"
                        ".........\n");
  dbuf second;
  buffer_parse (&second, "###");
  char *expected = ".........\n"
                   "...###...\n"
                   ".........";

  // when:
  buffer_merge (&first, &second, 1, 3);
  dstr actual = buffer_to_dstr (&first);

  // then:
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
merge_to_empty_buffers_test ()
{
  /* It should add empty rows and fill padding by spaces */

  // given:
  dbuf first = DBUF_EMPTY;
  dbuf second;
  buffer_parse (&second, "###");
  char *expected = "\n"
                   "   ###";

  // when:
  buffer_merge (&first, &second, 1, 3);
  dstr actual = buffer_to_dstr (&first);

  // then:
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
merge_bigger_buffer_test ()
{
  // given:
  dbuf first;
  buffer_parse (&first, ".......\n"
                        ".......\n"
                        ".......");
  dbuf second;
  buffer_parse (&second, "###\n"
                         "######\n"
                         "###");
  char *expected = ".......\n"
                   "...###.\n"
                   "...######\n"
                   "   ###";

  // when:
  buffer_merge (&first, &second, 1, 3);
  dstr actual = buffer_to_dstr (&first);

  // then:
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
merge_utf_buffer_test ()
{
  // given:
  dbuf first;
  buffer_parse (&first, "███\n"
                        "███\n"
                        "███\n");
  dbuf second;
  buffer_parse (&second, "☺");
  char *expected = "███\n"
                   "█☺█\n"
                   "███";

  // when:
  buffer_merge (&first, &second, 1, 1);
  dstr actual = buffer_to_dstr (&first);

  // then:
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
empty_laby_test ()
{
  // given:
  dbuf buf = DBUF_EMPTY;
  level level = LEVEL_EMPTY;
  laby_init_empty (&level.lab, 1, 1);

  char *expected = "┏━━━┓\n"
                   "┃ @ ┃\n"
                   "┗━━━┛";
  // when:
  render_level (&level, &buf);
  // then:
  dstr actual = buffer_to_dstr (&buf);
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
simple_laby_test ()
{
  // given:
  dbuf buf = DBUF_EMPTY;
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
  dstr actual = buffer_to_dstr (&buf);
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
generate_eller_test ()
{
  // given:
  dbuf buf = DBUF_EMPTY;
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
  dstr actual = buffer_to_dstr (&buf);
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
all_tests ()
{
  mu_run_test (parse_string_to_buffer_test);
  mu_run_test (merge_buffers_test);
  mu_run_test (merge_to_empty_buffers_test);
  mu_run_test (merge_bigger_buffer_test);
  mu_run_test (merge_utf_buffer_test);
  mu_run_test (empty_laby_test);
  mu_run_test (simple_laby_test);
  mu_run_test (generate_eller_test);
  return 0;
}

int
main (void)
{
  char *result = all_tests ();
  if (result != 0)
    printf (SGR_RED "%s\n" SGR_RESET, result);
  else
    printf (SGR_GREEN "\nAll tests have been passed.\n" SGR_RESET);

  printf ("Tests run: %d\n", tests_run);

  return result != 0;
}
