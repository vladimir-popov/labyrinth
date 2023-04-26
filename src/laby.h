#ifndef __LABY__
#define __LABY__

/*
 * All information about a single room should be encoded in one byte:
 *
 * |   C   |L U R B|V|
 * |0 ... 0|0 0 0 0|0|
 *
 * Section C: describes a content of the room;
 * Section L: describes the left border of the room;
 * Section U: describes the upper border of the room;
 * Section R: describes the right border of the room;
 * Section B: describes the bottom border of the room;
 * Section V: was room visited by player or not;
 */
typedef unsigned int room;

#define BORDERS_BITS_COUNT 4
#define VISIT_BITS_COUNT 1

#define CONTENT_SHIFT (BORDERS_BITS_COUNT + VISIT_BITS_COUNT)

enum border
{
  BOTTOM_BORDER = 1 << VISIT_BITS_COUNT,
  RIGHT_BORDER = 2 << VISIT_BITS_COUNT,
  UPPER_BORDER = 4 << VISIT_BITS_COUNT,
  LEFT_BORDER = 8 << VISIT_BITS_COUNT,
};

/* The single horizontal line of rooms. */
typedef room *row;

/* The structure described a labyrinth */
typedef struct laby
{
  /* The count of rooms by horizontal */
  int cols_count;

  /* The count of rooms by vertical */
  int rows_count;

  /* The rows of the labyrinth's rooms */
  row *rooms;
} laby;

/* Creates a new labyrinth with cols x rows empty rooms. */
laby
laby_init_empty (int rows, int cols);

/* Frees memory of the labyrinth. */
void
laby_free (laby *lab);

/*  Returns only 4 first bits, which are about borders of the room. */
unsigned char
laby_get_border (laby *lab, int row, int col);

/* Add border flag. */
void
laby_add_border (laby *lab, int row, int col, enum border border);

/* Remove border flag. */
void
laby_rm_border (laby *lab, int row, int col, enum border border);

void
laby_mark_as_visited (laby *lab, int r, int c);

char
laby_is_visited (laby *lab, int r, int c);

void
laby_set_content (laby *lab, int r, int c, unsigned char value);

unsigned char
laby_get_content (laby *lab, int r, int c);

laby
laby_generate (int rows, int cols, int seed);


#endif /* __LABY__ */