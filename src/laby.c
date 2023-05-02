/**
 * This is the representation of the current game
 * field, which includes the size and borders of the labyrinth, information
 * about the visited rooms, items, creatures and so on.
 */
#include "laby.h"
#include <stdio.h>
#include <stdlib.h>

/* Creates a new labyrinth with height x width empty rooms. */
void
laby_init_empty (laby *lab, int height, int width)
{
  lab->height = height;
  lab->width = width;
  lab->rooms = malloc (sizeof (row) * height);
  for (int i = 0; i < height; i++)
    lab->rooms[i] = calloc (width, sizeof (room));
}

/* Frees memory of the labyrinth. */
void
laby_free (laby *lab)
{
  for (int i = 0; i < lab->height; i++)
    free (lab->rooms[i]);

  free (lab->rooms);
}

/*  Returns only 4 first bits, which are about borders of the room. */
unsigned char
laby_get_border (laby *lab, int y, int x)
{
  int is_x_inside = x >= 0 && x < lab->width;
  int is_y_inside = y >= 0 && y < lab->height;

  int border = 0;

  if (is_y_inside && is_x_inside)
    {
      border = lab->rooms[y][x];
      if (y == 0)
        border |= UPPER_BORDER;
      if (x == 0)
        border |= LEFT_BORDER;
      if (y == lab->height - 1)
        border |= BOTTOM_BORDER;
      if (x == lab->width - 1)
        border |= RIGHT_BORDER;
    }
  else if (is_x_inside && y == -1)
    border = BOTTOM_BORDER;
  else if (is_x_inside && y == lab->height)
    border = UPPER_BORDER;
  else if (is_y_inside && x == -1)
    border = RIGHT_BORDER;
  else if (is_y_inside && x == lab->width)
    border = LEFT_BORDER;
  else
    border = 0;

  return border & (0xf << VISIT_BITS_COUNT);
}

/* Add border flag. */
void
laby_add_border (laby *lab, int y, int x, enum border border)
{
  lab->rooms[y][x] |= border;
  /* also, we should set appropriate borders for neighbors */
  if (border & RIGHT_BORDER)
    if (x < lab->width - 1)
      lab->rooms[y][x + 1] |= LEFT_BORDER;
  if (border & BOTTOM_BORDER)
    if (y < lab->height - 1)
      lab->rooms[y + 1][x] |= UPPER_BORDER;
  if (border & LEFT_BORDER)
    if (x > 0)
      lab->rooms[y][x - 1] |= RIGHT_BORDER;
  if (border & UPPER_BORDER)
    if (y > 0)
      lab->rooms[y - 1][x] |= BOTTOM_BORDER;
}

/* Remove border flag. */
void
laby_rm_border (laby *lab, int y, int x, enum border border)
{
  lab->rooms[y][x] &= ~border;
  /* also, we should set appropriate borders for neighbors */
  if (border & RIGHT_BORDER)
    if (x < lab->width - 1)
      lab->rooms[y][x + 1] &= ~LEFT_BORDER;
  if (border & BOTTOM_BORDER)
    if (y < lab->height - 1)
      lab->rooms[y + 1][x] &= ~UPPER_BORDER;
  if (border & LEFT_BORDER)
    if (x > 0)
      lab->rooms[y][x - 1] &= ~RIGHT_BORDER;
  if (border & UPPER_BORDER)
    if (y > 0)
      lab->rooms[y - 1][x] &= ~BOTTOM_BORDER;
}

void
laby_mark_as_visited (laby *lab, int r, int c)
{
  lab->rooms[r][c] |= 1;
}

char
laby_is_visited (laby *lab, int y, int x)
{
  int is_y_inside = y >= 0 && y < lab->height;
  int is_x_inside = x >= 0 && x < lab->width;
  return (is_y_inside && is_x_inside) ? lab->rooms[y][x] & 1 : 0;
}

void
laby_set_content (laby *lab, int y, int x, unsigned char value)
{
  int mask = (1 << CONTENT_SHIFT) - 1;
  lab->rooms[y][x] = (value << CONTENT_SHIFT) | (lab->rooms[y][x] & mask);
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
void
laby_generate (laby *lab, int height, int width, int seed)
{
  srand (seed);

  /* The final labyrinth */
  laby_init_empty (lab, height, width);

  /* Sets of rooms. The first one is a sets for the current row, the second is
   * for the next row */
  char *s = malloc (sizeof (char) * width);
  char *_s = malloc (sizeof (char) * width);

  int set = 1;
  /* set unique set for every empty room in the first row */
  for (int j = 0; j < width; j++)
    _s[j] = set++;

  for (int y = 0; y < height - 1; y++)
    {
      /* swap sets to use `s` for the current row */
      char *tmp = s;
      s = _s;
      _s = tmp;

      /* decide if two rooms should have a horizontal border */
      for (int x = 0; x < width - 1; x++)
        {
          if (s[x] != s[x + 1] && rand () % 2 == 0)
            laby_add_border (lab, y, x, RIGHT_BORDER);
          else
            s[x + 1] = s[x];
        }
      /* decide if two rooms should have a vertical border */
      for (int x = 0; x < width; x++)
        {
          /* count of rooms without bottom border in the current set */
          int no_bb = 0;
          for (int i = 0; i < width && no_bb < 2; i++)
            if (s[x] == s[i])
              no_bb = (laby_get_border (lab, y, i) & BOTTOM_BORDER)
                          ? no_bb
                          : no_bb + 1;

          /* we can create a border, if it's not a single room without bottom
           * border in the set */
          if (no_bb > 1 && rand () % 5 > 0)
            {
              laby_add_border (lab, y, x, BOTTOM_BORDER);
              /* mark the underlining room to change its set */
              _s[x] = set++;
            }
          else
            _s[x] = s[x];
        }
    }
  /* remove dead ends */
  for (int x = 0; x < width - 1; x++)
    laby_rm_border (lab, height - 1, x, RIGHT_BORDER);
}
