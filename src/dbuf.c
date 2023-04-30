#include "dbuf.h"
#include "term.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static dstr
dstr_init (const char *template, int len)
{
  char *str = malloc (len);
  strncpy (str, template, len);
  dstr new = { len, str };
  return new;
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
dstr_free (const dstr *str)
{
  free (str->chars);
}

dbuf
buffer_init (const char *str)
{
  dstr *lines = malloc (sizeof (dstr));
  lines[0] = dstr_init (str, strlen (str));
  dbuf new = { 1, 0, lines };
  return new;
}

void
buffer_free (dbuf *buf)
{
  for (int i = 0; i < buf->lines_count; i++)
    {
      dstr_free (&buf->lines[i]);
    }
}

void
buffer_add_line (dbuf *buf, const char *str, int len)
{
  dstr *more_rows
      = realloc (buf->lines, sizeof (dstr) * (buf->lines_count + 1));

  if (more_rows == NULL)
    return;

  more_rows[buf->lines_count] = dstr_init (str, len);

  buf->lines = more_rows;
  buf->lines_count++;
  buf->last_line_ended = 0;
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

dbuf
buffer_parse (const char *str)
{
  dbuf buf = DBUF_EMPTY;
  char *s = malloc (sizeof (char) * strlen (str));
  strcpy (s, str);

  char *next = strtok (s, "\r\n");
  while (next != NULL)
    {
      buffer_add_line (&buf, next, strlen (next));
      next = strtok (NULL, "\r\n");
    }
  return buf;
}

dstr
buffer_to_dstr (const dbuf *buf)
{
  dstr res = DSTR_EMPTY;
  for (int i = 0; i < buf->lines_count; i++)
    {
      dstr line = buf->lines[i];
      dstr_append (&res, line.chars, line.length);
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
buffer_merge (dbuf *first, const dbuf *second, int rowpad, int colpad)
{
  /* Iterate over intersected lines and merge them */
  for (int i = rowpad, j = 0;
       i < first->lines_count && j < second->lines_count; i++, j++)
    {
      int n = (second->lines[j].length + colpad > first->lines[i].length)
                  ? first->lines[i].length - colpad
                  : second->lines[j].length;

      /* Copy chars from line to line */
      char *ptr = &first->lines[i].chars[colpad];
      for (int k = 0; k < n; k++, ptr++)
        *ptr = second->lines[j].chars[k];
    }
}
