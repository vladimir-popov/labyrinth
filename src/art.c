#include "art.h"
#include "u8.h"
#include <string.h>

void
art_create_frame (u8buf *buf, int height, int width, const art_border b)
{
  for (int i = 0; i < height; i++)
    {
      u8str str = U8_STR_EMPTY;
      if (i == 0)
        {
          u8_str_append (&str, b.luc, strlen (b.luc));
          u8_str_append_repeate (&str, b.hor, strlen (b.hor), width - 2);
          u8_str_append (&str, b.ruc, strlen (b.ruc));
        }
      else if (i == height - 1)
        {
          u8_str_append (&str, b.lbc, strlen (b.lbc));
          u8_str_append_repeate (&str, b.hor, strlen (b.hor), width - 2);
          u8_str_append (&str, b.rbc, strlen (b.rbc));
        }
      else
        {
          u8_str_append (&str, b.vert, strlen (b.vert));
          u8_str_append_repeate (&str, b.bg, strlen (b.bg), width - 2);
          u8_str_append (&str, b.vert, strlen (b.vert));
        }
      u8_buffer_add_line (buf, str.chars, str.length);
    }
}
