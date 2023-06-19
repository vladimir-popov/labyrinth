#include <string.h>

extern int tests_run;
extern char *test_only;

#define mu_assert(message, test)                                              \
  do                                                                          \
    {                                                                         \
      if (!(test))                                                            \
        return message;                                                       \
    }                                                                         \
  while (0)

#define mu_u8str_eq_to_str(actual_u8str, expected_str)                        \
  mu_assert (                                                                 \
      (actual_u8str.chars != NULL) ? actual_u8str.chars : "Empty u8str",      \
      actual_u8str.length > 0                                                 \
          && memcmp (actual_u8str.chars, expected_str, actual_u8str.length)   \
                 == 0)

#define mu_run_test(test)                                                     \
  do                                                                          \
    {                                                                         \
      if (test_only == NULL || strstr (#test, test_only) != NULL)             \
        {                                                                     \
          char *message = test ();                                            \
          tests_run++;                                                        \
          if (message)                                                        \
            {                                                                 \
              printf ("\x1b[31m - " #test " has been failed:\n");             \
              return message;                                                 \
            }                                                                 \
          else                                                                \
            printf ("\x1b[32m + " #test " has been passed.\x1b[31m\n");       \
        }                                                                     \
    }                                                                         \
  while (0)
