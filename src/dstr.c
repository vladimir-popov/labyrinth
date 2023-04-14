#include <stdlib.h>
#include <string.h>

/******************** Dynamic string *********************/
typedef struct dstr
{
  int length;
  char *chars;
} dstr;

#define DSTR_EMPTY                                                            \
  {                                                                           \
    0, NULL                                                                   \
  }

dstr
dstr_init (const char *template)
{
  int len = strlen (template);
  char *str = malloc (len);
  strcpy (str, template);
  dstr new = { len, str };
  return new;
}

void
dstr_append (dstr *this, const char *prefix, int len)
{
  char *new = realloc (this->chars, this->length + len);

  if (new == NULL)
    return;

  memcpy (&new[this->length], prefix, len);
  this->chars = new;
  this->length += len;
}

void
dstr_free (const dstr *str)
{
  free (str->chars);
}
