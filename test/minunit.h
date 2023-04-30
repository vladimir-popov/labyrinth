#include <string.h>

#define mu_assert(message, test)                                              \
  do                                                                          \
    {                                                                         \
      if (!(test))                                                            \
        return message;                                                       \
    }                                                                         \
  while (0)

#define mu_dstr_eq_to_str(actual_dstr, expected_str)                          \
  mu_assert (actual_dstr.chars, strcmp (expected_str, actual_dstr.chars) == 0)

#define mu_run_test(test)                                                     \
  do                                                                          \
    {                                                                         \
      char *message = test ();                                                \
      tests_run++;                                                            \
      if (message)                                                            \
        {                                                                     \
          printf ("\x1b[31m - " #test " has been failed:\n");                 \
          return message;                                                     \
        }                                                                     \
    }                                                                         \
  while (0)

extern int tests_run;
