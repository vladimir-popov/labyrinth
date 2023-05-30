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
  if (source[*idx] == '\x1b')
    do
      {
        (*idx)++;
      }
    while (source[*idx] == '[' || source[*idx] == 'H');

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
u8_str_init (U8_Str *str, const char *template, int len)
{
  str->chars = malloc (len);
  strncpy (str->chars, template, len);
  str->length = len;
}

void
u8_str_append (U8_Str *dest, const char *prefix, int len)
{
  dest->chars = realloc (dest->chars, dest->length + len);
  memcpy (&dest->chars[dest->length], prefix, len);
  dest->length += len;
}

void
u8_str_append_str (U8_Str *dest, const char *prefix)
{
  u8_str_append (dest, prefix, strlen (prefix));
}

void
u8_str_append_repeate (U8_Str *ds, const char *str, int len, int count)
{
  int i = ds->length;
  ds->length += sizeof (char) * count * len;
  ds->chars = realloc (ds->chars, ds->length);
  for (; i < ds->length; i += len)
    memcpy (&ds->chars[i], str, len);
}

void
u8_str_append_repeate_str (U8_Str *ds, const char *str, int count)
{
  u8_str_append_repeate (ds, str, strlen (str), count);
}

void
u8_str_merge (U8_Str *dest, const U8_Str *source, int spad)
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
u8_str_free (const U8_Str *str)
{
  free (str->chars);
}

void
u8_buffer_init (U8_Buf *buf, const char *str)
{
  U8_Str *lines = malloc (sizeof (U8_Str));
  u8_str_init (&lines[0], str, strlen (str));
  buf->lines = lines;
  buf->lines_count = 1;
  buf->last_line_ended = 0;
}

void
u8_buffer_free (U8_Buf *buf)
{
  for (int i = 0; i < buf->lines_count; i++)
    {
      u8_str_free (&buf->lines[i]);
    }
}

static void
u8_buffer_add_str_line (U8_Buf *buf, const U8_Str str)
{
  buf->lines = realloc (buf->lines, sizeof (U8_Str) * (buf->lines_count + 1));
  buf->lines[buf->lines_count] = str;
  buf->lines_count++;
  buf->last_line_ended = 0;
}

void
u8_buffer_add_line (U8_Buf *buf, const char *str, int len)
{
  U8_Str ds;
  u8_str_init (&ds, str, len);
  u8_buffer_add_str_line (buf, ds);
}

void
u8_buffer_append_str (U8_Buf *buf, const char *str, int len)
{
  if (buf->last_line_ended || buf->lines == NULL)
    u8_buffer_add_line (buf, str, len);
  else
    u8_str_append (&buf->lines[buf->lines_count - 1], str, len);
}

void
u8_buffer_end_line (U8_Buf *buf)
{
  buf->last_line_ended = 1;
}

void
u8_buffer_parse (U8_Buf *buf, const char *str)
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

U8_Str
u8_buffer_to_u8str (const U8_Buf *buf)
{
  U8_Str res = U8_STR_EMPTY;
  for (int i = 0; i < buf->lines_count; i++)
    {
      U8_Str *line = &buf->lines[i];
      if (line->length > 0)
        u8_str_append (&res, line->chars, line->length);

      if (i < buf->lines_count - 1)
        u8_str_append (&res, "\n", 1);
    }

  return res;
}

void
u8_buffer_write (int fildes, const U8_Buf *buf, int rowpad, int colpad,
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
      U8_Str *line = &buf->lines[i];
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
u8_buffer_merge (U8_Buf *dest, const U8_Buf *source, int rowpad, int colpad)
{
  /* Add empty lines if rowpad great than lines_count */
  while (dest->lines_count <= rowpad)
    {
      U8_Str ds = U8_STR_EMPTY;
      u8_buffer_add_str_line (dest, ds);
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
      U8_Str ds = U8_STR_EMPTY;
      u8_str_append_repeate (&ds, " ", 1, colpad);
      u8_str_append (&ds, source->lines[j].chars, source->lines[j].length);
      u8_buffer_add_str_line (dest, ds);
    }
}
