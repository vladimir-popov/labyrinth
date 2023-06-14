/**
 * This is the representation of the current game
 * field, which includes the size and borders of the labyrinth, information
 * about the visited rooms, items, creatures and so on.
 */
#include "laby.h"
#include "2d_math.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define VISIBLE_MASK 0x10
#define KNOWN_MASK 0x20

#define CONTENT_SHIFT 6

#define min(a, b) ((a < b) ? a : b)
#define max(a, b) ((a > b) ? a : b)

/* Creates a new labyrinth with height x width empty rooms. */
void
laby_init_empty (Laby *lab, int height, int width)
{
  lab->rows = height;
  lab->cols = width;
  lab->rooms = malloc (sizeof (row) * height);
  for (int i = 0; i < height; i++)
    lab->rooms[i] = calloc (width, sizeof (room));
}

/* Frees memory of the labyrinth. */
void
laby_free (Laby *lab)
{
  for (int i = 0; i < lab->rows; i++)
    free (lab->rooms[i]);

  free (lab->rooms);
}

/*  Returns only 4 first bits, which are about borders of the room. */
unsigned char
laby_get_borders (const Laby *lab, int r, int c)
{
  int is_x_inside = c >= 0 && c < lab->cols;
  int is_y_inside = r >= 0 && r < lab->rows;

  int border = 0;

  if (is_y_inside && is_x_inside)
    {
      border = lab->rooms[r][c];
      if (r == 0)
        border |= UPPER_BORDER;
      if (c == 0)
        border |= LEFT_BORDER;
      if (r == lab->rows - 1)
        border |= BOTTOM_BORDER;
      if (c == lab->cols - 1)
        border |= RIGHT_BORDER;
    }
  else if (is_x_inside && r == -1)
    border = BOTTOM_BORDER;
  else if (is_x_inside && r == lab->rows)
    border = UPPER_BORDER;
  else if (is_y_inside && c == -1)
    border = RIGHT_BORDER;
  else if (is_y_inside && c == lab->cols)
    border = LEFT_BORDER;
  else
    border = 0;

  return border & 0xf;
}

/* Add border flag. */
void
laby_add_border (Laby *lab, int y, int x, enum border border)
{
  lab->rooms[y][x] |= border;
  /* also, we should set appropriate borders for neighbors */
  if (border & RIGHT_BORDER)
    if (x < lab->cols - 1)
      lab->rooms[y][x + 1] |= LEFT_BORDER;
  if (border & BOTTOM_BORDER)
    if (y < lab->rows - 1)
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
laby_rm_border (Laby *lab, int r, int c, enum border border)
{
  lab->rooms[r][c] &= ~border;
  /* also, we should set appropriate borders for neighbors */
  if (border & RIGHT_BORDER)
    if (c < lab->cols - 1)
      lab->rooms[r][c + 1] &= ~LEFT_BORDER;
  if (border & BOTTOM_BORDER)
    if (r < lab->rows - 1)
      lab->rooms[r + 1][c] &= ~UPPER_BORDER;
  if (border & LEFT_BORDER)
    if (c > 0)
      lab->rooms[r][c - 1] &= ~RIGHT_BORDER;
  if (border & UPPER_BORDER)
    if (r > 0)
      lab->rooms[r - 1][c] &= ~BOTTOM_BORDER;
}

_Bool
laby_is_visible (const Laby *lab, int r, int c)
{
  int is_x_inside = c >= 0 && c < lab->cols;
  int is_y_inside = r >= 0 && r < lab->rows;

  return (is_y_inside && is_x_inside) ? lab->rooms[r][c] & VISIBLE_MASK : 0;
}

void
laby_set_visibility (Laby *lab, int r, int c, _Bool flag)
{
  int is_x_inside = c >= 0 && c < lab->cols;
  int is_y_inside = r >= 0 && r < lab->rows;

  if (is_y_inside && is_x_inside)
    {
      if (flag)
        lab->rooms[r][c] |= (VISIBLE_MASK | KNOWN_MASK);
      else
        lab->rooms[r][c] &= ~VISIBLE_MASK;
    }
}

_Bool
laby_is_known_room (const Laby *lab, int r, int c)
{
  int is_x_inside = c >= 0 && c < lab->cols;
  int is_y_inside = r >= 0 && r < lab->rows;

  return (is_y_inside && is_x_inside) ? lab->rooms[r][c] & KNOWN_MASK : 0;
}

void
laby_mark_as_known (Laby *lab, int r, int c)
{
  int is_x_inside = c >= 0 && c < lab->cols;
  int is_y_inside = r >= 0 && r < lab->rows;

  if (is_y_inside && is_x_inside)
    lab->rooms[r][c] |= KNOWN_MASK;
}

void
laby_set_content (Laby *lab, int y, int x, enum content value)
{
  int mask = (1 << CONTENT_SHIFT) - 1;
  lab->rooms[y][x] = (value << CONTENT_SHIFT) | (lab->rooms[y][x] & mask);
}

unsigned char
laby_get_content (const Laby *lab, int r, int c)
{
  return lab->rooms[r][c] >> CONTENT_SHIFT;
}

/**
 * Calculates border lines for labyrinth with 3x3 room size.
 */
static int
laby_get_borders_lines (const Laby *lab, const int r, const int c, int borders,
                        Line dest[])
{
  int y = 3 * r;
  int x = 3 * c;
  enum border all_borders[4]
      = { LEFT_BORDER, UPPER_BORDER, RIGHT_BORDER, BOTTOM_BORDER };

  int j = 0;
  for (int i = 0; i < 4; i++)
    {
      enum border border = all_borders[i] & borders;
      if (border == 0)
        continue;
      switch (border)
        {
        case BOTTOM_BORDER:
          dest[j].p0.y = y + 3;
          dest[j].p0.x = x;
          dest[j].p1.y = y + 3;
          dest[j].p1.x = x + 3;
          break;
        case RIGHT_BORDER:
          dest[j].p0.y = y;
          dest[j].p0.x = x + 3;
          dest[j].p1.y = y + 3;
          dest[j].p1.x = x + 3;
          break;
        case UPPER_BORDER:
          dest[j].p0.y = y;
          dest[j].p0.x = x;
          dest[j].p1.y = y;
          dest[j].p1.x = x + 3;
          break;
        case LEFT_BORDER:;
          dest[j].p0.y = y;
          dest[j].p0.x = x;
          dest[j].p1.y = y + 3;
          dest[j].p1.x = x;
          break;
        }
      j++;
    }
  return j;
}

static _Bool
is_intersect_with_borders (Laby *lab, int fy, int fx, int ty, int tx)
{
  Line blines[8];
  int borders = laby_get_borders (lab, fy / 3, fx / 3);
  int bcount = laby_get_borders_lines (lab, fy / 3, fx / 3, borders, blines);
  borders = laby_get_borders (lab, ty / 3, tx / 3);
  bcount += laby_get_borders_lines (lab, ty / 3, tx / 3, borders,
                                    &blines[bcount]);
  for (int i = 0; i < bcount; i++)
    {
      if (line_is_intersectedp (&blines[i], fx, fy, tx, ty))
        return 1;
    }
  return 0;
}

static void
mark_visible_in_direction (Laby *lab, int r0, int c0, int r1, int c1)
{
  /* middle of the 'from' room */
  int y0 = 3 * r0 + 1;
  int x0 = 3 * c0 + 1;
  /* middle of the 'to' room */
  int y1 = 3 * r1 + 1;
  int x1 = 3 * c1 + 1;

  /* Bresenham's line algorithm.
   *  See https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm */
  int dy = -abs (y1 - y0);
  int sy = (y0 < y1) ? 1 : -1;
  int dx = abs (x1 - x0);
  int sx = (x0 < x1) ? 1 : -1;
  int error = dx + dy;

  int y = y0;
  int x = x0;
  while (1)
    {

      /* if we achieved a new room */
      if (r0 != y / 3 || c0 != x / 3)
        {
          r0 = y / 3;
          c0 = x / 3;

          if (is_intersect_with_borders (lab, y0, x0, y, x))
            break;

          laby_set_visibility (lab, r0, c0, 1);
          y0 = 3 * r0 + 1;
          x0 = 3 * c0 + 1;
        }

      if (y == y1 && x == x1)
        break;

      int e2 = 2 * error;
      if (e2 >= dy)
        {
          if (x == x1)
            break;
          error += dy;
          x += sx;
        }
      if (e2 <= dx)
        {
          if (y == y1)
            break;
          error += dx;
          y += sy;
        }
    }
}

void
laby_mark_visible_rooms (Laby *lab, int r, int c, int range)
{
  /* initial room must be visible */
  laby_set_visibility (lab, r, c, 1);

  /* goes around perimeter of the visible aria and issues glance from the
   * middle of the room r:c to the middle of rooms in the visible range */
  for (int i = 0; i <= 2 * range; i++)
    {
      int r1 = min (lab->rows - 1, max (0, r - range + i));
      int c1 = max (0, c - range);
      mark_visible_in_direction (lab, r, c, r1, c1);
      c1 = min (lab->cols - 1, c + range);
      mark_visible_in_direction (lab, r, c, r1, c1);

      if (i > 0 && i < 2 * range)
        {
          r1 = max (0, r - range);
          c1 = min (lab->cols - 1, max (0, c - range + i));
          mark_visible_in_direction (lab, r, c, r1, c1);
          r1 = min (lab->rows - 1, r + range);
          mark_visible_in_direction (lab, r, c, r1, c1);
        }
    }
}

/*
 * This is an implementation of the Eller's algorithm.
 *
 * @see
 * https://weblog.jamisbuck.org/2010/12/29/maze-generation-eller-s-algorithm
 * @see http://www.neocomputer.org/projects/eller.html
 */
void
laby_generate (Laby *lab, int height, int width, int seed)
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
              no_bb = (laby_get_borders (lab, y, i) & BOTTOM_BORDER)
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
