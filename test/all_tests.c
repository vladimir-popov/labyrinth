#include "rtmterm_tests.c"
#include "u8_tests.c"

char *
all_tests ()
{
  mu_run_test (u8_all_tests);
  mu_run_test (rtm_all_tests);
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
