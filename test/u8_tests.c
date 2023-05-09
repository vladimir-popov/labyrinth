#include <stdio.h>
#include <stdlib.h>

#include "minunit.h"
#include "u8.h"

/*
 * Ⓐ  0xE2 0x92 0xB6
 * Ⓑ  0xE2 0x92 0xB7
 * Ⓒ  0xE2 0x92 0xB8
 * Ⓓ  0xE2 0x92 0xB9
 * ☺  0xE2 0x98 0xBA
 * █	0xE2 0x96 0x88
 * ░	0xE2 0x96 0x91
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
utf8_str_merge_1_test ()
{
  /* Here we check a case, when the destination line
   * has more symbols, but less bytes than the source */

  // given:
  u8str dest;
  u8_str_init (&dest, "!   !", 5);
  u8str source;
  u8_str_init (&source, "███", 9);
  char *expected = "!███!";
  // when:
  u8_str_merge (&dest, &source, 1);
  // then:
  mu_u8str_eq_to_str (dest, expected);
  mu_assert ("Wrong length in the result", dest.length == 11);
  return 0;
}

static char *
utf8_str_merge_2_test ()
{
  /* Here we check a case, when the destination line
   * has more symbols, and more bytes than the source */

  // given:
  u8str dest;
  u8_str_init (&dest, "███", 9);
  u8str source;
  u8_str_init (&source, "☺", 3);
  char *expected = "█☺█";
  // when:
  u8_str_merge (&dest, &source, 1);
  // then:
  mu_u8str_eq_to_str (dest, expected);
  mu_assert ("Wrong length in the result", dest.length == 9);
  return 0;
}

static char *
utf8_merge_into_empty_str_test ()
{
  // given:
  u8str dest = U8_STR_EMPTY;
  u8str source;
  u8_str_init (&source, "☺", 3);
  char *expected = " ☺";
  // when:
  u8_str_merge (&dest, &source, 1);
  // then:
  mu_u8str_eq_to_str (dest, expected);

  char *msg = malloc (sizeof (char) * 45);
  sprintf (msg, "Wrong length in the result: %d, but expected 4", dest.length);
  mu_assert (msg, dest.length == 4);
  free (msg);
  return 0;
}

/* ========= Tests for buffer ========== */

static char *
parse_string_to_buffer_test ()
{
  // given:
  char *template = "This is\n"
                   "a template";
  // when:
  u8buf buf = U8_BUF_EMPTY;
  u8_buffer_parse (&buf, template);
  u8str res = u8_buffer_to_u8str (&buf);

  // then:
  mu_u8str_eq_to_str (res, template);
  return 0;
}

static char *
merge_into_empty_buffers_test ()
{
  /* It should add empty rows and fill padding by spaces */

  // given:
  u8buf first = U8_BUF_EMPTY;
  u8buf second = U8_BUF_EMPTY;
  u8_buffer_parse (&second, "###");
  char *expected = "\n"
                   "   ###";

  // when:
  u8_buffer_merge (&first, &second, 1, 3);
  u8str actual = u8_buffer_to_u8str (&first);

  // then:
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
merge_middle_buffer_test ()
{
  // given:
  u8buf first = U8_BUF_EMPTY;
  u8_buffer_parse (&first, ".........\n"
                           ".........\n"
                           ".........\n");
  u8buf second = U8_BUF_EMPTY;
  u8_buffer_parse (&second, "###");
  char *expected = ".........\n"
                   "...###...\n"
                   ".........";

  // when:
  u8_buffer_merge (&first, &second, 1, 3);
  u8str actual = u8_buffer_to_u8str (&first);

  // then:
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
merge_bigger_buffer_test ()
{
  // given:
  u8buf first = U8_BUF_EMPTY;
  u8_buffer_parse (&first, ".......\n"
                           ".......\n"
                           ".......");
  u8buf second = U8_BUF_EMPTY;
  u8_buffer_parse (&second, "###\n"
                            "######\n"
                            "###");
  char *expected = ".......\n"
                   "...###.\n"
                   "...######\n"
                   "   ###";

  // when:
  u8_buffer_merge (&first, &second, 1, 3);
  u8str actual = u8_buffer_to_u8str (&first);

  // then:
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}

static char *
merge_utf_buffer_test ()
{
  // given:
  u8buf first = U8_BUF_EMPTY;
  u8_buffer_parse (&first, "███\n"
                           "███\n"
                           "███\n");
  u8buf second = U8_BUF_EMPTY;
  u8_buffer_parse (&second, "☺");
  char *expected = "███\n"
                   "█☺█\n"
                   "███";

  // when:
  u8_buffer_merge (&first, &second, 1, 1);
  u8str actual = u8_buffer_to_u8str (&first);

  // then:
  mu_u8str_eq_to_str (actual, expected);
  return 0;
}
