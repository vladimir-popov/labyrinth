#include "2d_math_tests.c"
#include "render_tests.c"
#include "u8_tests.c"
#include "term.h"
#include <stdio.h>

int tests_run = 0;
char *test_only = NULL;
// char *test_only = "laby_visibility_test_3";

char *
all_tests ()
{
  /* 2d math tests */
  mu_run_test(intersected_lines_test);
  mu_run_test(parallel_lines_intersection_test);
  mu_run_test(perpendicular_lines_intersection_test);
  mu_run_test(lines_intersection_test_1);
  /* str tests */
  mu_run_test (utf8_find_index_test);
  mu_run_test (utf8_symbols_count_test);
  mu_run_test (utf8_find_symbol_test);
  mu_run_test (utf8_str_merge_1_test);
  mu_run_test (utf8_str_merge_2_test);
  mu_run_test (utf8_merge_into_empty_str_test);
  mu_run_test (utf8_str_crop_test_1);
  mu_run_test (utf8_str_crop_test_2);
  mu_run_test (utf8_str_crop_test_3); 
  mu_run_test (utf8_str_crop_test_4);
  /* buffer tests */
  mu_run_test (parse_string_to_buffer_test);
  mu_run_test (merge_into_empty_buffers_test);
  mu_run_test (merge_middle_buffer_test);
  mu_run_test (merge_bigger_buffer_test);
  mu_run_test (merge_utf_buffer_test);
  mu_run_test (crop_utf_buffer_test);
  mu_run_test (fill_utf_buffer_test);
  // /* laby tests */
  mu_run_test (empty_laby_test);
  mu_run_test (simple_laby_test);
  mu_run_test (generate_eller_test);
  mu_run_test (render_laby_with_player_test);
  mu_run_test (visibility_in_open_space_test);
  mu_run_test (visibility_in_closed_space_test_1);
  mu_run_test (visibility_in_closed_space_test_2);
  mu_run_test (laby_visibility_crossroads_test);
  mu_run_test (laby_visibility_test_1);
  mu_run_test (laby_visibility_test_2);
  mu_run_test (laby_visibility_test_3);
  return 0;
}

int
main (void)
{
  if (test_only == NULL)
    printf (SGR_GREEN "Run tests...\n" SGR_RED);
  else
    printf (SGR_GREEN "Run only %s...\n" SGR_RED, test_only);

  char *result = all_tests ();
  if (result != 0)
    printf (SGR_RED "%s\n", result);
  else
    printf (SGR_GREEN "\nAll tests have been passed.\n");

  printf ("Tests run: %d\n" SGR_RESET, tests_run);

  return result != 0;
}
