#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * Section V: visiting of the room; 0 - not visited; 1 - visited;
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
laby_init_empty (int rows, int cols)
{
  laby l = { cols, rows, NULL };
  l.rooms = malloc (sizeof (row) * rows);
  for (int i = 0; i < rows; i++)
    l.rooms[i] = calloc (cols, sizeof (room));

  return l;
}

/* Frees memory of all rooms. */
void
laby_free (laby *lab)
{
  for (int i = 0; i < lab->rows_count; i++)
    free (lab->rooms[i]);

  free (lab->rooms);
}

/*  Returns only 4 first bits, which are about borders of the room. */
unsigned char
laby_get_border (laby *lab, int row, int col)
{
  int is_col_inside = col >= 0 && col < lab->cols_count;
  int is_row_inside = row >= 0 && row < lab->rows_count;

  int border = 0;

  if (is_row_inside && is_col_inside)
    {
      border = lab->rooms[row][col];
      if (row == 0)
        border |= UPPER_BORDER;
      if (col == 0)
        border |= LEFT_BORDER;
      if (row == lab->rows_count - 1)
        border |= BOTTOM_BORDER;
      if (col == lab->cols_count - 1)
        border |= RIGHT_BORDER;
    }
  else if (is_col_inside && row == -1)
    border = BOTTOM_BORDER;
  else if (is_col_inside && row == lab->rows_count)
    border = UPPER_BORDER;
  else if (is_row_inside && col == -1)
    border = RIGHT_BORDER;
  else if (is_row_inside && col == lab->cols_count)
    border = LEFT_BORDER;
  else
    border = 0;

  return border & (0xf << VISIT_BITS_COUNT);
}

/* Add border flag. */
void
laby_add_border (laby *lab, int row, int col, enum border border)
{
  lab->rooms[row][col] |= border;
  /* also, we should set appropriate borders for neighbors */
  if (border & RIGHT_BORDER)
    if (col < lab->cols_count - 1)
      lab->rooms[row][col + 1] |= LEFT_BORDER;
  if (border & BOTTOM_BORDER)
    if (row < lab->rows_count - 1)
      lab->rooms[row + 1][col] |= UPPER_BORDER;
  if (border & LEFT_BORDER)
    if (col > 0)
      lab->rooms[row][col - 1] |= RIGHT_BORDER;
  if (border & UPPER_BORDER)
    if (row > 0)
      lab->rooms[row - 1][col] |= BOTTOM_BORDER;
}

/* Remove border flag. */
void
laby_rm_border (laby *lab, int row, int col, enum border border)
{
  lab->rooms[row][col] &= ~border;
  /* also, we should set appropriate borders for neighbors */
  if (border & RIGHT_BORDER)
    if (col < lab->cols_count - 1)
      lab->rooms[row][col + 1] &= ~LEFT_BORDER;
  if (border & BOTTOM_BORDER)
    if (row < lab->rows_count - 1)
      lab->rooms[row + 1][col] &= ~UPPER_BORDER;
  if (border & LEFT_BORDER)
    if (col > 0)
      lab->rooms[row][col - 1] &= ~RIGHT_BORDER;
  if (border & UPPER_BORDER)
    if (row > 0)
      lab->rooms[row - 1][col] &= ~BOTTOM_BORDER;
}

void
laby_mark_as_visit (laby *lab, int r, int c)
{
  lab->rooms[r][c] |= 1;
}

char
laby_is_visit(laby *lab, int r, int c)
{
  return lab->rooms[r][c] & 1;
}

void
laby_set_content (laby *lab, int r, int c, unsigned char value)
{
  int mask = (1 << CONTENT_SHIFT) - 1;
  lab->rooms[r][c] = (value << CONTENT_SHIFT) | (lab->rooms[r][c] & mask);
}

unsigned char
laby_get_content (laby *lab, int r, int c)
{
  return lab->rooms[r][c] >> CONTENT_SHIFT;
}

#define get(R, C) laby_get_content (&lab, (R), (C))
#define set(R, C, V) laby_set_content (&lab, (R), (C), (V))
laby
laby_generate_eller (int rows, int cols, int seed)
{
  srand (seed);

  /* The final labyrinth */
  laby lab = laby_init_empty (rows, cols);

  int set = 1;
  /* set unique set for every empty room in the first row */
  for (int j = 0; j < cols; j++)
    set (0, j, set++);

  for (int r = 0; r < rows - 1; r++)
    {
      /* decide if two rooms should have a horizontal border */
      for (int c = 0; c < cols - 1; c++)
        {
          if (get (r, c) != get (r, c + 1) && rand () % 2 == 0)
            laby_add_border (&lab, r, c, RIGHT_BORDER);
          else
            set (r, c + 1, get (r, c));
        }
      /* decide if two rooms should have a vertical border */
      for (int c = 0; c < cols; c++)
        {
          /* count of rooms without bottom border in the current set */
          int no_bb = 0;
          for (int i = 0; i < cols && no_bb < 2; i++)
            if (get (r, c) == get (r, i))
              no_bb = (laby_get_border (&lab, r, i) & BOTTOM_BORDER)
                          ? no_bb
                          : no_bb + 1;

          /* we can create a border, if it's not a single room without bottom
           * border in the set */
          if (no_bb > 1 && rand () % 5 > 0)
            {
              laby_add_border (&lab, r, c, BOTTOM_BORDER);
              /* mark the underlining room to change its set */
              set (r + 1, c, set++);
            }
          else
            set (r + 1, c, get (r, c));
        }
    }
  /* remove dead ends */
  for (int c = 0; c < cols - 1; c++)
    laby_rm_border (&lab, rows - 1, c, RIGHT_BORDER);

  return lab;
}
#undef get
#undef set

/* ========== DEBUG FUNCTIONS ========== */
void
laby_print_raw (laby *lab)
{
  for (int r = 0; r < lab->rows_count; r++)
    {
      for (int c = 0; c < lab->cols_count; c++)
        printf ("%d ", lab->rooms[r][c]);

      printf ("\r\n");
    }
}

void
laby_print_borders (laby *lab)
{
  for (int r = -1; r <= lab->rows_count; r++)
    {
      for (int c = -1; c <= lab->cols_count; c++)
        printf ("%2d ", laby_get_border (lab, r, c));

      printf ("\r\n");
    }
}

void
laby_print_values (laby *lab)
{
  for (int r = 0; r < lab->rows_count; r++)
    {
      for (int c = 0; c < lab->cols_count; c++)
        printf ("%2d ", laby_get_content (lab, r, c));

      printf ("\r\n");
    }
}
