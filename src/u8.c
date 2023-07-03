#include "u8.h"
#include "term.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
u8_find_symbol (const char *source, int len, int *idx)
{
  /* move to the nearest symbol beginning */
  while ((*idx < len) && (source[*idx] & 0xC0) == 0x80)
    (*idx)++;

  /* looks like no one full symbol till the end */
  if (*idx == len)
    return 0;

  /* skip control sequence */
  // TODO skip CS more generally
  if (source[*idx] == '\x1b' && source[++(*idx)] == '[')
    {
      (*idx)++;
      while (*idx < len)
        {
          char c = source[*idx];
          (*idx)++;
          if (c == 'B' || c == 'C' || c == 'J' || c == 'H' || c == 'h'
              || c == 'm' || c == 'l' || c == 'n')
            break;
        }
    }

  /* count symbol's bytes */
  int n = 1;
  while ((*idx + n) < len && (source[*idx + n] & 0xC0) == 0x80)
    n++;

  return n;
}

int
u8_find_index (const char *source, int len, int n)
{
  int found = 0;
  int i = 0;
  while (found < n && u8_find_symbol (source, len, &i))
    {
      found++;
      i++;
    }
  return (found == n) ? i - 1 : -1;
}

int
u8_symbols_count (const char *source, int len)
{
  int i = 0, n = 0;
  while (i < len && u8_find_symbol (source, len, &i))
    {
      n++;
      i++;
    }
  return n;
}

void
u8_str_init (u8str *str, const char *template, int len)
{
  str->chars = malloc (len);
  strncpy (str->chars, template, len);
  str->length = len;
}

void
u8_str_append (u8str *dest, const char *prefix, int len)
{
  dest->chars = realloc (dest->chars, dest->length + len);
  memcpy (&dest->chars[dest->length], prefix, len);
  dest->length += len;
}

void
u8_str_append_str (u8str *dest, const char *prefix)
{
  u8_str_append (dest, prefix, strlen (prefix));
}

int
u8_str_symbols_count (u8str *str)
{
  int found = 0;
  int i = 0;
  while (u8_find_symbol (str->chars, str->length, &i))
    {
      found++;
      i++;
    }
  return found;
}

void
u8_str_append_repeate (u8str *ds, const char *str, int len, int count)
{
  int i = ds->length;
  ds->length += sizeof (char) * count * len;
  ds->chars = realloc (ds->chars, ds->length);
  for (; i < ds->length; i += len)
    memcpy (&ds->chars[i], str, len);
}

void
u8_str_append_repeate_str (u8str *ds, const char *str, int count)
{
  u8_str_append_repeate (ds, str, strlen (str), count);
}

void
u8_str_merge (u8str *dest, const u8str *source, int spad)
{
  /* find an start index of the spad + 1 symbol
   * from which replacement should become */
  int sx = u8_find_index (dest->chars, dest->length, spad + 1);

  /* Add space till spad, if dest's length less than spad */
  if (sx < 0)
    {
      int scount = u8_symbols_count (dest->chars, dest->length);
      for (int i = spad - scount; i > 0; i--)
        {
          u8_str_append (dest, " ", 1);
        }
      sx = dest->length;
    }

  /* Now, let's find an end index of a symbol till which
   * replacement should happened */
  int slen = u8_symbols_count (source->chars, source->length);
  int ex = u8_find_index (dest->chars, dest->length, slen + spad + 1);

  if (ex < 0)
    {
      /* it means that destination should be completely replaced
       * right after sx */
      dest->length = sx + source->length;
      dest->chars = realloc (dest->chars, dest->length);
      memcpy (&dest->chars[sx], source->chars, source->length);
    }
  else
    {
      /* in this case we have a small appendix at the end of the destination,
       * which should be moved right after the inserted source string */
      int aplen = dest->length - ex;
      dest->length = sx + source->length + aplen;
      dest->chars = realloc (dest->chars, dest->length);
      memcpy (&dest->chars[sx + source->length], &dest->chars[ex], aplen);
      memcpy (&dest->chars[sx], source->chars, source->length);
    }
}

void
u8_str_crop (u8str *str, int pad, int count)
{
  int pad_idx
      = (pad == 0) ? 0 : u8_find_index (str->chars, str->length, pad + 1);
  /* if pad_idx < 0 means that pad is great than count of symbols in the str */
  if (pad_idx < 0 || count == 0)
    {
      u8_str_clean (str);
    }
  else if (pad == 0)
    {
      int length = u8_find_index (str->chars, str->length, count + 1);
      if (length > 0 && length < str->length)
        {
          str->chars = realloc (str->chars, length);
          str->length = length;
        }
    }
  else
    {
      int length = str->length - pad_idx;
      char *chars = malloc (sizeof (char) * length);
      memcpy (chars, &str->chars[pad_idx], sizeof (char) * length);
      free (str->chars);
      str->chars = chars;
      str->length = length;
      u8_str_crop (str, 0, count);
    }
}

void
u8_str_clean (u8str *str)
{
  free (str->chars);
  str->chars = NULL;
  str->length = 0;
}

void
u8_str_free (const u8str *str)
{
  free (str->chars);
}

void
u8_buffer_init (u8buf *buf, const char *str)
{
  u8str *lines = malloc (sizeof (u8str));
  u8_str_init (&lines[0], str, strlen (str));
  buf->lines = lines;
  buf->lines_count = 1;
  buf->last_line_ended = 0;
}

void
u8_buffer_clean (u8buf *buf)
{
  buf->lines = NULL;
  buf->last_line_ended = 0;
  buf->lines_count = 0;
}

void
u8_buffer_free (u8buf *buf)
{
  for (int i = 0; i < buf->lines_count; i++)
    {
      u8_str_free (&buf->lines[i]);
    }
  free (buf->lines);
}

static void
u8_buffer_add_str_line (u8buf *buf, const u8str *str)
{
  buf->lines = realloc (buf->lines, sizeof (u8str) * (buf->lines_count + 1));
  buf->lines[buf->lines_count].chars = str->chars;
  buf->lines[buf->lines_count].length = str->length;
  buf->lines_count++;
  buf->last_line_ended = 0;
}

void
u8_buffer_add_line (u8buf *buf, const char *str, int len)
{
  u8str ds;
  u8_str_init (&ds, str, len);
  u8_buffer_add_str_line (buf, &ds);
}

void
u8_buffer_append_str (u8buf *buf, const char *str, int len)
{
  if (buf->last_line_ended || buf->lines == NULL)
    u8_buffer_add_line (buf, str, len);
  else
    u8_str_append (&buf->lines[buf->lines_count - 1], str, len);
}

void
u8_buffer_end_line (u8buf *buf)
{
  buf->last_line_ended = 1;
}

void
u8_buffer_parse (u8buf *buf, const char *str)
{
  char *s = malloc (sizeof (char) * strlen (str));
  strcpy (s, str);

  char *next = strtok (s, "\r\n");
  while (next != NULL)
    {
      u8_buffer_append_str (buf, next, strlen (next));
      u8_buffer_end_line (buf);
      next = strtok (NULL, "\r\n");
    }
  free (s);
}

u8str
u8_buffer_to_u8str (const u8buf *buf)
{
  u8str res = U8_STR_EMPTY;
  for (int i = 0; i < buf->lines_count; i++)
    {
      u8str *line = &buf->lines[i];
      if (line->length > 0)
        u8_str_append (&res, line->chars, line->length);

      if (i < buf->lines_count - 1)
        u8_str_append (&res, "\n", 1);
    }

  return res;
}

void
u8_buffer_crop (u8buf *buf, int rowpad, int colpad, int height, int width)
{
  if (rowpad >= buf->lines_count || height == 0)
    {
      u8_buffer_clean (buf);
      return;
    }

  if (rowpad > 0)
    {
      int lines_count = buf->lines_count - rowpad;
      lines_count = (lines_count < height) ? lines_count : height;
      u8str *lines = malloc (sizeof (u8str) * lines_count);
      memcpy (lines, &buf->lines[rowpad], sizeof (u8str) * lines_count);
      free (buf->lines);
      buf->lines = lines;
      buf->lines_count = lines_count;
    }
  for (int i = 0; i < buf->lines_count; i++)
    {
      u8_str_crop (&buf->lines[i], colpad, width);
    }
}

void
u8_buffer_write (int fildes, const u8buf *buf, int rowpad, int colpad,
                 int height, int width)
{
  for (int i = 0; i < buf->lines_count && i < height; i++)
    {
      int len;
      /* move cursor according to padding */
      if (rowpad > 0 || colpad > 0)
        {
          char *cup;
          len = set_cursor_position (&cup, rowpad + i + 1, colpad + 1);
          write (fildes, cup, len);
          free (cup);
        }
      u8str *line = &buf->lines[i];
      /* Cut line */
      len = u8_find_index (line->chars, line->length, width + 1);
      len = (len < 0) ? line->length : len;
      write (fildes, line->chars, len);
      /* Break line */
      if (i < buf->lines_count - 1)
        write (fildes, "\n", 1);
    }
}

void
u8_buffer_replace_str (u8buf *buf, int dest_row, u8str *source)
{
  if (dest_row >= buf->lines_count || dest_row < 0)
    return;
  u8_str_free (&buf->lines[dest_row]);
  buf->lines[dest_row].chars = source->chars;
  buf->lines[dest_row].length = source->length;
}

void
u8_buffer_fill (u8buf *buf, char *ch, int ch_len, int height, int width)
{
  int dh = height - buf->lines_count;
  dh = (dh > 0) ? dh : buf->lines_count;

  for (int i = 0; i < dh; i++)
    {
      int dw = width - u8_str_symbols_count (&buf->lines[i]);
      if (dw <= 0)
        continue;
      u8_str_append_repeate (&buf->lines[i], ch, ch_len, dw);
    }

  for (int i = buf->lines_count; i < height; i++)
    {
      u8str str = U8_STR_EMPTY;
      u8_str_append_repeate (&str, ch, ch_len, width);
      u8_buffer_add_str_line (buf, &str);
    }
}

void
u8_buffer_merge (u8buf *dest, const u8buf *source, int rowpad, int colpad)
{
  /* Add empty lines if rowpad great than lines_count */
  while (dest->lines_count <= rowpad)
    {
      u8str ds = U8_STR_EMPTY;
      u8_buffer_add_str_line (dest, &ds);
    }

  /* Iterate over intersected lines and merge them */
  int i = rowpad, j = 0;
  for (; i < dest->lines_count && j < source->lines_count; i++, j++)
    {
      u8_str_merge (&dest->lines[i], &source->lines[j], colpad);
    }

  /* Just copy extra lines from the second buffer */
  for (; j < source->lines_count; j++)
    {
      u8str ds = U8_STR_EMPTY;
      u8_str_append_repeate (&ds, " ", 1, colpad);
      u8_str_append (&ds, source->lines[j].chars, source->lines[j].length);
      u8_buffer_add_str_line (dest, &ds);
    }
}
