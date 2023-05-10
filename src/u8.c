#include "u8.h"
#include "term.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
u8_find_symbol (const char *source, int len, int *bix)
{
  /* move to the nearest symbol beginning */
  while ((*bix < len) && (source[*bix] & 0xC0) == 0x80)
    (*bix)++;

  /* looks like no one full symbol till the end */
  if (*bix == len)
    return 0;

  /* count symbol's bytes */
  int n = 1;
  while ((*bix + n) < len && (source[*bix + n] & 0xC0) == 0x80)
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

static void
u8_str_repeate (u8str *ds, const char ch, int count)
{
  ds->chars = malloc (sizeof (char) * count);
  ds->length = count;
  for (int i = 0; i < count; i++)
    ds->chars[i] = ch;
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
u8_buffer_free (u8buf *buf)
{
  for (int i = 0; i < buf->lines_count; i++)
    {
      u8_str_free (&buf->lines[i]);
    }
}

static void
u8_buffer_add_str_line (u8buf *buf, const u8str str)
{
  buf->lines = realloc (buf->lines, sizeof (u8str) * (buf->lines_count + 1));
  buf->lines[buf->lines_count] = str;
  buf->lines_count++;
  buf->last_line_ended = 0;
}

void
u8_buffer_add_line (u8buf *buf, const char *str, int len)
{
  u8str ds;
  u8_str_init (&ds, str, len);
  u8_buffer_add_str_line (buf, ds);
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
      u8_buffer_add_line (buf, next, strlen (next));
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
u8_buffer_write (int fildes, const u8buf *buf, int rowpad, int colpad,
                 int width, int height)
{
  for (int i = 0; i < buf->lines_count && i < height; i++)
    {
      int len;
      char cup[10];
      /* move cursor according to padding */
      if (rowpad > 0)
        {
          len = set_cursor_position (cup, rowpad + i, colpad);
          write (fildes, cup, len);
        }
      u8str *line = &buf->lines[i];
      len = u8_find_index (line->chars, line->length, width + 1);
      len = (len < 0) ? line->length : len;
      write (fildes, line->chars, len);
      if (i < buf->lines_count - 1)
        write (fildes, "\n", 1);
    }
}

void
u8_buffer_merge (u8buf *dest, const u8buf *source, int rowpad, int colpad)
{
  /* Add empty lines if rowpad great than lines_count */
  while (dest->lines_count <= rowpad)
    {
      u8str ds = U8_STR_EMPTY;
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
      u8str ds;
      u8_str_repeate (&ds, ' ', colpad);
      u8_str_append (&ds, source->lines[j].chars, source->lines[j].length);
      u8_buffer_add_str_line (dest, ds);
    }
}
