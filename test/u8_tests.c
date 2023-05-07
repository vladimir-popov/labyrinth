#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "u8.h"

int tests_run = 0;

/*
 * Ⓐ  0xE2 0x92 0xB6
 * Ⓑ  0xE2 0x92 0xB7
 * Ⓒ  0xE2 0x92 0xB8
 * Ⓓ  0xE2 0x92 0xB9
 * ☺  0xE2 0x98 0xBA
 */

static char *
utf8_find_index_test ()
{
  // given:
  const char *utf_str = "ⒶⒷⒸⒹ";
  // when:
  int index1 = u8_find_index (utf_str, strlen (utf_str), 1);
  int index2 = u8_find_index (utf_str, strlen (utf_str), 2);
  // then:
  char *msg = malloc (sizeof (char) * 38);
  sprintf (msg, "Wrong index of the first symbol: %3d", index1);
  mu_assert (msg, index1 == 0);

  sprintf (msg, "Wrong index of the second symbol: %3d", index2);
  mu_assert (msg, index2 == 3);
  return 0;
}

static char *
utf8_symbols_count_test ()
{
  // given:
  const char *utf_str = "ⒶⒷⒸⒹ";
  // when:
  int count = u8_symbols_count (utf_str, strlen (utf_str));
  // then:
  mu_assert ("Wrong count", count == 4);
  return 0;
}

static char *
utf8_find_symbol_test ()
{
  // given:
  const char *utf_str = "ⒶⒷⒸⒹ";
  int bix = 1;
  // when:
  int size = u8_find_symbol (utf_str, strlen (utf_str), &bix);
  // then:
  mu_assert ("Wrong position of the symbol", bix == 3);
  mu_assert ("Wrong size of the symbol", size == 3);
  return 0;
}

static char *
parse_string_to_buffer_test ()
{
  // given:
  char *template = "This is\n"
                   "a template";
  // when:
  u8buf buf;
  u8_buffer_parse (&buf, template);
  u8str res = u8_buffer_to_dstr (&buf);

  // then:
  mu_dstr_eq_to_str (res, template);
  return 0;
}

static char *
merge_to_empty_buffers_test ()
{
  /* It should add empty rows and fill padding by spaces */

  // given:
  u8buf first = U8_BUF_EMPTY;
  u8buf second;
  u8_buffer_parse (&second, "###");
  char *expected = "\n"
                   "   ###";

  // when:
  u8_buffer_merge (&first, &second, 1, 3);
  u8str actual = u8_buffer_to_dstr (&first);

  // then:
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
merge_middle_buffer_test ()
{
  // given:
  u8buf first;
  u8_buffer_parse (&first, ".........\n"
                           ".........\n"
                           ".........\n");
  u8buf second;
  u8_buffer_parse (&second, "###");
  char *expected = ".........\n"
                   "...###...\n"
                   ".........";

  // when:
  u8_buffer_merge (&first, &second, 1, 3);
  u8str actual = u8_buffer_to_dstr (&first);

  // then:
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
merge_bigger_buffer_test ()
{
  // given:
  u8buf first;
  u8_buffer_parse (&first, ".......\n"
                           ".......\n"
                           ".......");
  u8buf second;
  u8_buffer_parse (&second, "###\n"
                            "######\n"
                            "###");
  char *expected = ".......\n"
                   "...###.\n"
                   "...######\n"
                   "   ###";

  // when:
  u8_buffer_merge (&first, &second, 1, 3);
  u8str actual = u8_buffer_to_dstr (&first);

  // then:
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

static char *
merge_utf_buffer_test ()
{
  // given:
  u8buf first;
  u8_buffer_parse (&first, "███\n"
                           "███\n"
                           "███\n");
  u8buf second;
  u8_buffer_parse (&second, "☺");
  char *expected = "███\n"
                   "█☺█\n"
                   "███";

  // when:
  u8_buffer_merge (&first, &second, 1, 1);
  u8str actual = u8_buffer_to_dstr (&first);

  // then:
  mu_dstr_eq_to_str (actual, expected);
  return 0;
}

char *
u8_all_tests ()
{
  mu_run_test (utf8_find_index_test);
  mu_run_test (utf8_symbols_count_test);
  mu_run_test (utf8_find_symbol_test);
  mu_run_test (parse_string_to_buffer_test);
  mu_run_test (merge_to_empty_buffers_test);
  mu_run_test (merge_middle_buffer_test);
  mu_run_test (merge_bigger_buffer_test);
  mu_run_test (merge_utf_buffer_test);
  return 0;
}
