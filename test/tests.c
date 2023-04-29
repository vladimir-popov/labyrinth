#include <stdio.h>

#include "game.h"
#include "minunit.h"
#include "render.c"
#include "term.c"

int tests_run = 0;

static char *
parse_string_to_buffer_test ()
{
  // given:
  char *template = "This is\n"
                   "a template";
  // when:
  dbuf buf = buffer_parse (template);
  dstr res = buffer_to_dstr (&buf);

  // then:
  mu_dstr_eq_to_str (res, template);
  return 0;
}

static char *
empty_laby_test ()
{
  // given:
  dbuf buf = DBUF_EMPTY;
  level level = LEVEL_EMPTY;
  level.lab = laby_init_empty (1, 1);

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
  level.lab = laby_init_empty (3, 3);
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
  level.lab = laby_generate (3, 5, 1);
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

/* ========== DEBUG FUNCTIONS ========== */
void
laby_print_raw (laby *lab)
{
  for (int r = 0; r < lab->rows_count; r++)
    {
      for (int c = 0; c < lab->cols_count; c++)
        printf ("%d ", lab->rooms[r][c]);

      printf ("\n");
    }
}

void
laby_print_borders (laby *lab)
{
  for (int r = -1; r <= lab->rows_count; r++)
    {
      for (int c = -1; c <= lab->cols_count; c++)
        printf ("%2d ", laby_get_border (lab, r, c));

      printf ("\n");
    }
}

void
laby_print_contents (laby *lab)
{
  for (int r = 0; r < lab->rows_count; r++)
    {
      for (int c = 0; c < lab->cols_count; c++)
        printf ("%2d ", laby_get_content (lab, r, c));

      printf ("\n");
    }
}
