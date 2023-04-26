/**
 * This is the representation of the current game
 * field, which includes the size and borders of the labyrinth, information
 * about the visited rooms, items, creatures and so on.
 */
#include <stdlib.h>
#include <stdio.h>
#include "laby.h"


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

/* Frees memory of the labyrinth. */
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
laby_mark_as_visited (laby *lab, int r, int c)
{
  lab->rooms[r][c] |= 1;
}

char
laby_is_visited (laby *lab, int r, int c)
{
  int is_col_inside = c >= 0 && c < lab->cols_count;
  int is_row_inside = r >= 0 && r < lab->rows_count;
  return (is_row_inside && is_col_inside) ? lab->rooms[r][c] & 1 : 0;
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

/*
 * This is an implementation of the Eller's algorithm.
 *
 * @see
 * https://weblog.jamisbuck.org/2010/12/29/maze-generation-eller-s-algorithm
 * @see http://www.neocomputer.org/projects/eller.html
 */
laby
laby_generate (int rows, int cols, int seed)
{
  srand (seed);

  /* The final labyrinth */
  laby lab = laby_init_empty (rows, cols);

  /* Sets of rooms. The first one is a sets for the current row, the second is
   * for the next row */
  char *s = malloc(sizeof(char) * cols);
  char *_s = malloc(sizeof(char) * cols);

  int set = 1;
  /* set unique set for every empty room in the first row */
  for (int j = 0; j < cols; j++)
    _s[j] = set++;

  for (int r = 0; r < rows - 1; r++)
    {
      /* swap sets to use `s` for the current row */
      char *tmp = s;
      s = _s;
      _s = tmp;

      /* decide if two rooms should have a horizontal border */
      for (int c = 0; c < cols - 1; c++)
        {
          if (s[c] != s[c + 1] && rand () % 2 == 0)
            laby_add_border (&lab, r, c, RIGHT_BORDER);
          else
            s[c + 1] = s[c];
        }
      /* decide if two rooms should have a vertical border */
      for (int c = 0; c < cols; c++)
        {
          /* count of rooms without bottom border in the current set */
          int no_bb = 0;
          for (int i = 0; i < cols && no_bb < 2; i++)
            if (s[c] == s[i])
              no_bb = (laby_get_border (&lab, r, i) & BOTTOM_BORDER)
                          ? no_bb
                          : no_bb + 1;

          /* we can create a border, if it's not a single room without bottom
           * border in the set */
          if (no_bb > 1 && rand () % 5 > 0)
            {
              laby_add_border (&lab, r, c, BOTTOM_BORDER);
              /* mark the underlining room to change its set */
              _s[c] = set++;
            }
          else
            _s[c] = s[c];
        }
    }
  /* remove dead ends */
  for (int c = 0; c < cols - 1; c++)
    laby_rm_border (&lab, rows - 1, c, RIGHT_BORDER);

  return lab;
}
