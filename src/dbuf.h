
#ifndef __DYNAMIC_BUFFER__
#define __DYNAMIC_BUFFER__

/**
 * Dynamic string which can be dynamically extended by another string.
 */
typedef struct
{
  int length;
  char *chars;
} dstr;

#define DSTR_EMPTY                                                            \
  {                                                                           \
    0, NULL                                                                   \
  }

/**
 * The list of string lines which can be dynamically extended.
 */
typedef struct
{
  int lines_count;
  int last_line_ended;
  dstr *lines;
} dbuf;

#define DBUF_EMPTY                                                            \
  {                                                                           \
    0, 0, NULL                                                                \
  }

/**
 * Creates a new buffer with single line `str`.
 */
dbuf buffer_init (const char *str);

/**
 * Splits the string to lines by the '\n' character.
 */
dbuf buffer_parse (const char *str);

/**
 * Appends the string to the last line in the buffer. The string should not
 * contain '\r', nor '\n' symbols.
 */
void buffer_append_str (dbuf *buf, const char *str, int len);

/**
 * Inserts the string as a new line.
 */
void buffer_add_line (dbuf *buf, const char *str, int len);

/**
 * Appends end of the line to the buffer. So, the next append
 * will add a new line, instead of append string to the last line
 * of the buffer.
 */
void buffer_end_line (dbuf *buf);

/**
 * Merges all lines to the single string with '\r\n' separator.
 */
dstr buffer_to_dstr (const dbuf *buf);

/**
 * Writes the buffer to the object referenced by the descriptor fildes.
 * Similar to `write(fields, buffer_to_str(...))`, but more effective.
 */
void buffer_write (int fildes, const dbuf *buf);

/**
 * Writes the second buffer uppon the first with specified padding.
 */
void buffer_merge (dbuf *first, const dbuf *second, int rowpad, int colpad);

void buffer_free (dbuf *buf);

#endif /* __DYNAMIC_BUFFER__ */
