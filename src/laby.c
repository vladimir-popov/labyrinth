#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * All information about a single room should be encoded in one byte:
 *
 * |   C   |L U R B|
 * |0 0 0 0|0 0 0 0|
 *
 * Section C: describes content of the room;
 * Section L: describes the left border of the room;
 * Section U: describes the upper border of the room;
 * Section R: describes the right border of the room;
 * Section B: describes the bottom border of the room;
 */
typedef unsigned char room;

enum border
{
  BOTTOM_BORDER = 1,
  RIGHT_BORDER = 2,
  UPPER_BORDER = 4,
  LEFT_BORDER = 8,
};

#define BORDER_MASK = 0xf;

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

  return border & 0xf;
}

/* Add border flag and return update flag value */
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

laby
laby_generate_eller (int rows, int cols, int seed)
{
  srand (seed);

  /* The final labyrinth */
  laby lab = laby_init_empty (rows, cols);

  /* Sets of rooms */
  unsigned char *sets = malloc (cols * sizeof (unsigned char));
  unsigned char *_sets = malloc (cols * sizeof (unsigned char));

  /* Set unique set number for every room at the first row */
  for (unsigned char j = 0; j < cols; j++)
    sets[j] = j + 1;

  for (int r = 0; r < rows - 1; r++)
    {
      /* decide if two rooms should have a horizontal border */
      for (int c = 0; c < cols - 1; c++)
        {
          if (rand () % 2 == 0)
            laby_add_border (&lab, r, c, RIGHT_BORDER);
          else
            sets[c + 1] = sets[c];
        }
      /* we will generate bottom borders according to the sets for the
       * current line and change sets for the room on next row */
      memcpy (_sets, sets, cols);
      /* decide if two rooms should have a vertical border */
      for (int c = 0; c < cols - 1; c++)
        {
          if (rand () % 2 == 0)
            {
              /* count of rooms without bottom border in the current set */
              int n = 0;
              for (int i = c + 1; i < cols && _sets[i] == _sets[c] && n == 0;
                   i++)
                n++;
              for (int i = c - 1; i >= 0 && _sets[i] == _sets[c] && n == 0;
                   i++)
                n = (lab.rooms[r][c] | BOTTOM_BORDER) ? n : n + 1;
              /* we can create a border, if exists at least one room in the
               * same set with bottom border */
              if (n > 0)
                {
                  laby_add_border (&lab, r, c, BOTTOM_BORDER);
                  /* change the set of the underlining room
                   * (should not be equal to the right room set) */
                  sets[c] = (c < cols - 1) ? _sets[c + 1] + 1 : _sets[c] + 1;
                }
            }
        }
    }

  free (sets);
  free (_sets);

  return lab;
}

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
