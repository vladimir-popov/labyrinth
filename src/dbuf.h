
#ifndef __DYNAMIC_BUFFER__
#define __DYNAMIC_BUFFER__

/**
 * Dynamic string which can be dynamically extended by another string.
 */
typedef struct
{
  /* count of bytes in the string */
  int length;
  /* bytes array with symbols of the string */
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
  /* count of lines */
  int lines_count;
  /* if true, than a new line must be created on next append */
  int last_line_ended;
  /* array of lines */
  dstr *lines;
} dbuf;

#define DBUF_EMPTY                                                            \
  {                                                                           \
    0, 0, NULL                                                                \
  }

/*
 * Finds the nearest symbol to the bix-th byte and returns the count of bytes
 * of this symbol.
 *
 * @source the string with in utf-8 encoding.
 * @len the size in bytes of the source.
 * @bix byte index which will be increased to the beginning of the nearest
 *      utf-8 symbol in the source.
 * @return count of bytes of the symbol, or 0 if a symbol was not found.
 */
int u8_find_symbol (const char *source, int len, int *bix);

/* Returns the index of the n-th symbol (1-based), or -1 if symbols are not enough. */
int u8_find_index (const char *source, int len, int n);

int
u8_symbols_count (const char *source, int len);

/**
 * Creates a new buffer with a single line `str`.
 */
void buffer_init (dbuf *buf, const char *str);

/**
 * Splits the string to lines by the '\n' character.
 */
void buffer_parse (dbuf *buf, const char *str);

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
 * Similar to:
 * ```
 * dbuf *buf = ...
 * dstr s;
 * buffer_to_str(buf, &s);
 * write(fields, s.chars, s.length);
 * ```
 * , but more effective.
 */
void buffer_write (int fildes, const dbuf *buf);

/**
 * Writes the `source` buffer upon the `dest` with specified padding.
 *
 * @rowpad the number of lines from the upper border of the `dest` buffer,
 *         after which the lines from the `source` should be inserted to
 *         the `dest` buffer.
 * @colpad the count of symbols from the left border of the `dest` buffer,
 *         after which the symbols from the `source` line should be inserted.
 */
void buffer_merge (dbuf *dest, const dbuf *source, int rowpad, int colpad);

void buffer_free (dbuf *buf);

#endif /* __DYNAMIC_BUFFER__ */
