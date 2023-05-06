#include "dbuf.h"
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
  return i - 1;
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

static void
dstr_init (dstr *str, const char *template, int len)
{
  str->chars = malloc (len);
  strncpy (str->chars, template, len);
  str->length = len;
}

static void
dstr_append (dstr *this, const char *prefix, int len)
{
  char *new = realloc (this->chars, this->length + len);
  if (new == NULL)
    return;
  memcpy (&new[this->length], prefix, len);
  this->chars = new;
  this->length += len;
}

static void
dstr_repeate (dstr *ds, const char ch, int count)
{
  ds->chars = malloc (sizeof (char) * count);
  ds->length = count;
  for (int i = 0; i < count; i++)
    ds->chars[i] = ch;
}

static void
dstr_free (const dstr *str)
{
  free (str->chars);
}

void
buffer_init (dbuf *buf, const char *str)
{
  dstr *lines = malloc (sizeof (dstr));
  dstr_init (&lines[0], str, strlen (str));
  buf->lines = lines;
  buf->lines_count = 1;
  buf->last_line_ended = 0;
}

void
buffer_free (dbuf *buf)
{
  for (int i = 0; i < buf->lines_count; i++)
    {
      dstr_free (&buf->lines[i]);
    }
}

static void
buffer_add_dstr_line (dbuf *buf, const dstr str)
{
  dstr *more_rows
      = realloc (buf->lines, sizeof (dstr) * (buf->lines_count + 1));
  if (more_rows == NULL)
    return;

  more_rows[buf->lines_count] = str;

  buf->lines = more_rows;
  buf->lines_count++;
  buf->last_line_ended = 0;
}

void
buffer_add_line (dbuf *buf, const char *str, int len)
{
  dstr ds;
  dstr_init (&ds, str, len);
  buffer_add_dstr_line (buf, ds);
}

void
buffer_append_str (dbuf *buf, const char *str, int len)
{
  if (buf->last_line_ended || buf->lines == NULL)
    buffer_add_line (buf, str, len);
  else
    dstr_append (&buf->lines[buf->lines_count - 1], str, len);
}

void
buffer_end_line (dbuf *buf)
{
  buf->last_line_ended = 1;
}

void
buffer_parse (dbuf *buf, const char *str)
{
  buf->lines = NULL;
  buf->lines_count = 0;
  buf->last_line_ended = 0;

  char *s = malloc (sizeof (char) * strlen (str));
  strcpy (s, str);

  char *next = strtok (s, "\r\n");
  while (next != NULL)
    {
      buffer_add_line (buf, next, strlen (next));
      next = strtok (NULL, "\r\n");
    }
}

dstr
buffer_to_dstr (const dbuf *buf)
{
  dstr res = DSTR_EMPTY;
  for (int i = 0; i < buf->lines_count; i++)
    {
      dstr *line = &buf->lines[i];
      if (line->length > 0)
        dstr_append (&res, line->chars, line->length);

      if (i < buf->lines_count - 1)
        dstr_append (&res, "\n", 1);
    }

  return res;
}

void
buffer_write (int fildes, const dbuf *buf)
{
  for (int i = 0; i < buf->lines_count; i++)
    {
      dstr line = buf->lines[i];
      write (fildes, line.chars, line.length);
      if (i < buf->lines_count - 1)
        write (fildes, "\n", 1);
    }
}

void
buffer_merge (dbuf *dest, const dbuf *source, int rowpad, int colpad)
{
  /* Add empty lines if rowpad great than lines_count */
  while (dest->lines_count <= rowpad)
    {
      dstr ds = DSTR_EMPTY;
      buffer_add_dstr_line (dest, ds);
    }

  /* Iterate over intersected lines and merge them */
  int i = rowpad, j = 0;
  for (; i < dest->lines_count && j < source->lines_count; i++, j++)
    {
      dstr *l1 = &dest->lines[i];
      dstr *l2 = &source->lines[j];

      /* Add space till colpad, if line's length less then colpad */
      int slen1 = u8_symbols_count (l1->chars, l1->length);
      if (slen1 < colpad)
        {
          int len = colpad - slen1;
          dstr ds;
          dstr_repeate (&ds, ' ', len);
          dstr_append (&dest->lines[i], ds.chars, len);
          dstr_free (&ds);
        }

      /* Copy chars from line to line according to utf-8 encoding */

      int s1 = slen1 - colpad;
      int s2 = u8_symbols_count (l2->chars, l2->length);
      if (s1 > s2)
        { /* insert l2 into l1 */

          /* index of the byte in the l1 to which l2 should be inserted */
          int idx_from = u8_find_index (l1->chars, l1->length, colpad + 1);
          /* index of the byte from the l1 which should follow l2 */
          int idx_to = u8_find_index (l1->chars, l1->length, s2 + colpad + 1);
          /* add memory for the skipped bytes */
          if (l2->length > l1->length - idx_from)
            {
              l1->length = (idx_from + l1->length);
              l1->chars = realloc (l1->chars, l1->length);
            }
          memcpy (&l1->chars[idx_to], &l1->chars[idx_from],
                  (l1->length - idx_from));
          memcpy (&l1->chars[idx_from], l2->chars, l2->length);
        }
      else
        { /* just replace l1 from colpad till end by the l2 */

          int c1 = colpad;
          u8_find_symbol (l1->chars, l1->length, &c1);
          l1->length = l2->length + c1;
          l1->chars = realloc (l1->chars, l1->length);
          memcpy (&l1->chars[c1], l2->chars, l2->length);
        }
    }

  /* Just copy extra lines from the second buffer */
  for (; j < source->lines_count; j++)
    {
      dstr ds;
      dstr_repeate (&ds, ' ', colpad);
      dstr_append (&ds, source->lines[j].chars, source->lines[j].length);
      buffer_add_dstr_line (dest, ds);
    }
}
