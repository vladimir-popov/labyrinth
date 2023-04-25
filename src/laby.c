#include <stdio.h>

#include "laby.h"
#include "eller.c"

laby
laby_init_empty (int rows, int cols)
{
  laby l = { cols, rows, NULL };
  l.rooms = malloc (sizeof (row) * rows);
  for (int i = 0; i < rows; i++)
    l.rooms[i] = calloc (cols, sizeof (room));

  return l;
}

void
laby_free (laby *lab)
{
  for (int i = 0; i < lab->rows_count; i++)
    free (lab->rooms[i]);

  free (lab->rooms);
}

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

laby
laby_generate (int rows, int cols, int seed)
{
  return eller_generate(rows, cols, seed);
}

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
laby_print_contents (laby *lab)
{
  for (int r = 0; r < lab->rows_count; r++)
    {
      for (int c = 0; c < lab->cols_count; c++)
        printf ("%2d ", laby_get_content (lab, r, c));

      printf ("\r\n");
    }
}
