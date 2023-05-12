
#ifndef __U8__
#define __U8__

/*
 * Finds the nearest symbol to the n-th byte and returns the count of
 * bytes of this symbol. Control sequences do not count.
 *
 * @source a string within the utf-8 encoding.
 * @len a count of bytes of the source string.
 * @n the index of the byte which will be increased to the beginning of the
 *    nearest utf-8 symbol in the source, or kept the same, if n-th byte is a
 *    one byte symbol.
 * @return count of bytes of the symbol, or 0 if a symbol was not found.
 */
int u8_find_symbol (const char *source, int len, int *n);

/*
 * Returns the index of the n-th symbol (1-based), or -1 if symbols are
 * not enough. Control sequences do not count.
 */
int u8_find_index (const char *source, int len, int n);

/**
 * Returns a count of characters in utf-8 encoding in the string source
 * with length len. Control sequences do not count.
 */
int u8_symbols_count (const char *source, int len);

/**
 * The representation of the string in utf-8 encoding,
 * which can be dynamically extended by another utf-8 string.
 * instead of standard string representation this one doesn't
 * ended by the '\0' symbol.
 */
typedef struct
{
  /* count of bytes in the string */
  int length;
  /* bytes array */
  char *chars;
} u8str;

#define U8_STR_EMPTY                                                          \
  {                                                                           \
    0, NULL                                                                   \
  }

void u8_str_init (u8str *str, const char *template, int len);

void u8_str_append (u8str *dest, const char *prefix, int len);

void u8_str_append_repeate (u8str *ds, const char *str, int len, int count);

void u8_str_merge (u8str *dest, const u8str *source, int spad);

void u8_str_free (const u8str *str);

/**
 * The buffer of symbols in utf-8 encoding, represented as a list of u8str
 * lines.
 */
typedef struct
{
  /* count of lines */
  int lines_count;
  /* if true, than on next append a new line must be created */
  int last_line_ended;
  /* array of lines */
  u8str *lines;
} u8buf;

#define U8_BUF_EMPTY                                                          \
  {                                                                           \
    0, 0, NULL                                                                \
  }

/**
 * Creates a new buffer with a single line `str`.
 * The string str should be finished with '\0' symbol.
 */
void u8_buffer_init (u8buf *buf, const char *str);

/**
 * Splits the string to lines by the '\n' character.
 */
void u8_buffer_parse (u8buf *dest, const char *str);

/**
 * Appends the string to the last line in the buffer. The string should not
 * contain '\r', nor '\n' symbols.
 */
void u8_buffer_append_str (u8buf *buf, const char *str, int len);

/**
 * Inserts the string as a new line.
 */
void u8_buffer_add_line (u8buf *buf, const char *str, int len);

/**
 * Appends end of the line to the buffer. So, the next append
 * will add a new line, instead of append string to the last line
 * of the buffer.
 */
void u8_buffer_end_line (u8buf *buf);

/**
 * Merges all lines to the single string with '\n' separator.
 */
u8str u8_buffer_to_u8str (const u8buf *buf);

/**
 * Crops and writes with padding the buffer to the object referenced by the
 * descriptor fildes.
 * @rowpad count of symbols padding from top of the screen.
 * @colpad count of symbols padding from left side of the screen.
 * @height count of symbols which will be written by vertical after padding.
 * @width count of symbols which will be written by horizontal after padding.
 */
void u8_buffer_write (int fildes, const u8buf *buf, int rowpad, int colpad,
                      int height, int width);

/**
 * Writes the `source` buffer upon the `dest` with specified padding.
 *
 * @rowpad the number of lines from the upper border of the `dest` buffer,
 *         after which the lines from the `source` should be inserted to
 *         the `dest` buffer.
 * @colpad the count of symbols from the left border of the `dest` buffer,
 *         after which the symbols from the `source` line should be
 * inserted.
 */
void u8_buffer_merge (u8buf *dest, const u8buf *source, int rowpad,
                      int colpad);

void u8_buffer_free (u8buf *buf);

#endif /* __U8__ */
