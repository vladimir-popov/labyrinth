#include "art_tests.c"
#include "laby_tests.c"
#include "u8_tests.c"

int tests_run = 0;
char *test_only =NULL;

char *
all_tests ()
{
  /* str tests */
  mu_run_test (utf8_find_index_test);
  mu_run_test (utf8_symbols_count_test);
  mu_run_test (utf8_find_symbol_test);
  mu_run_test (utf8_str_merge_1_test);
  mu_run_test (utf8_str_merge_2_test);
  mu_run_test (utf8_merge_into_empty_str_test);
  /* buffer tests */
  mu_run_test (parse_string_to_buffer_test);
  mu_run_test (merge_into_empty_buffers_test);
  mu_run_test (merge_middle_buffer_test);
  mu_run_test (merge_bigger_buffer_test);
  mu_run_test (merge_utf_buffer_test);
  /* laby tests */
  mu_run_test (empty_laby_test);
  mu_run_test (simple_laby_test);
  mu_run_test (laby_visibility_test_1);
  mu_run_test (laby_visibility_test_2);
  mu_run_test (generate_eller_test);
  /* art tests */
  mu_run_test(create_frame_test);
  return 0;
}

int
main (void)
{
  if (test_only == NULL)
    printf (SGR_GREEN "Run tests...\n" SGR_RESET);
  else
    printf (SGR_GREEN "Run only %s...\n" SGR_RESET, test_only);

  char *result = all_tests ();
  if (result != 0)
    printf (SGR_RED "%s\n" SGR_RESET, result);
  else
    printf (SGR_GREEN "\nAll tests have been passed.\n" SGR_RESET);

  printf ("Tests run: %d\n", tests_run);

  return result != 0;
}
