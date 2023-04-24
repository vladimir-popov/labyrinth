#include "minunit.h"
#include "term.c"
#include "unicode_render.c"
#include <stdio.h>

int tests_run = 0;

static char *
empty_compact_laby_test ()
{
  // given:
  dstr buf = DSTR_EMPTY;
  laby lab = laby_init_empty (5, 5);

  char *expected = " ╔══╦══╦══╦══╗ \r\n"
                   " ╠══╬══╬══╬══╣ \r\n"
                   " ╠══╬══╬══╬══╣ \r\n"
                   " ╠══╬══╬══╬══╣ \r\n"
                   " ╚══╩══╩══╩══╝ ";
  // when:
  laby_render_compact (&buf, &lab);
  // then:
  mu_assert (buf.chars, strcmp (expected, buf.chars) == 0);
  return 0;
}

static char *
simple_compact_laby_test ()
{
  // given:
  dstr buf = DSTR_EMPTY;
  laby lab = laby_init_empty (3, 3);
  laby_add_border (&lab, 0, 1, (RIGHT_BORDER | BOTTOM_BORDER));
  laby_add_border (&lab, 1, 1, (LEFT_BORDER | BOTTOM_BORDER));
  laby_add_border (&lab, 2, 2, UPPER_BORDER);

  char *expected = " ╔═╕   ╓ \r\n"
                   " ║   ╘═╝ \r\n"
                   " ╚════╕  ";
  // when:
  laby_render_compact (&buf, &lab);
  // then:
  mu_assert (buf.chars, strcmp (expected, buf.chars) == 0);
  return 0;
}

static char *
empty_laby_test ()
{
  // given:
  dstr buf = DSTR_EMPTY;
  laby lab = laby_init_empty (1, 1);

  char *expected = "┏━━━┓\r\n"
                   "┃   ┃\r\n"
                   "┗━━━┛";
  // when:
  laby_render (&buf, &lab);
  // then:
  mu_assert (buf.chars, strcmp (expected, buf.chars) == 0);
  return 0;
}
static char *
simple_laby_test ()
{
  // given:
  dstr buf = DSTR_EMPTY;
  laby lab = laby_init_empty (3, 3);
  laby_add_border (&lab, 0, 1, (RIGHT_BORDER | BOTTOM_BORDER));
  laby_add_border (&lab, 1, 1, (LEFT_BORDER | BOTTOM_BORDER));
  laby_add_border (&lab, 2, 2, UPPER_BORDER);

  char *expected = 
"┏━━━━━━━┳━━━┓\r\n"
"┃       ┃   ┃\r\n"
"┃   ┏━━━┛   ┃\r\n"
"┃   ┃       ┃\r\n"
"┃   ┗━━━━━━━┫\r\n"
"┃           ┃\r\n"
"┗━━━━━━━━━━━┛";
  // when:
  laby_render (&buf, &lab);
  // then:
  mu_assert (buf.chars, strcmp (expected, buf.chars) == 0);
  return 0;
}

static char *
generate_eller_test()
{
  // given:
  dstr buf = DSTR_EMPTY;
  char *expected = 
"┏━━━━━━━━━━━━━━━┳━━━┓\r\n"
"┃               ┃   ┃\r\n"
"┃   ━━━━━━━━┳━━━┛   ┃\r\n"
"┃           ┃       ┃\r\n"
"┣━━━━━━━    ┗━━━    ┃\r\n"
"┃                   ┃\r\n"
"┗━━━━━━━━━━━━━━━━━━━┛";

  // when:
  laby lab = laby_generate_eller(3, 5, 1);
  // then:
  laby_render (&buf, &lab);
  mu_assert (buf.chars, strcmp (expected, buf.chars) == 0);
  return 0;
}

static char *
all_tests ()
{
  mu_run_test (empty_compact_laby_test);
  mu_run_test (simple_compact_laby_test);
  mu_run_test (empty_laby_test);
  mu_run_test (simple_laby_test);
  mu_run_test(generate_eller_test);
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
