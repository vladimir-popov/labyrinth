#include "art.h"
#include "minunit.h"
#include "u8.h"
#include <stdio.h>

char *
create_frame_test ()
{
  // given:
  u8buf buf = U8_BUF_EMPTY;
  art_border border = ART_SINGLE_BORDER;
  char *expected = "┏━━━┓\n"
                   "┃   ┃\n"
                   "┗━━━┛";
  // when:
  art_create_frame (&buf, 3, 5, border);
  u8str res = u8_buffer_to_u8str (&buf);
  // then:
  mu_u8str_eq_to_str (res, expected);
  return 0;
}
