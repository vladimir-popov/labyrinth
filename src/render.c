/**
 *  We will "draw" a game to the two dimension byte array as
 * indexes of the array with utf-8 symbols. It should help to avoid issues with
 * different size of symbols in utf-8 encoding. And at the end, we will
 * "render" this symbols map to the u8 buffer.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "2d_math.h"
#include "art.h"
#include "game.h"
#include "laby.h"
#include "render.h"
#include "u8.h"

// clang-format off
/* The array of symbols, which are used to render the game. */
static const char *symbols[] = 
//   0    1    2    3    4    5    6    7    8    9
  { 
     "", " ", "┃", "━", "┏", "┓", "┗", "┛", "╋", "┣",
    "┫", "┳", "┻", "@", "⛿", "·" 
  };
// clang-format on

void
smap_init (smap *sm, int rows, int cols, int room_height, int room_width)
{
  sm->height = rows * room_height + 1;
  sm->width = cols * room_width + 1;
  sm->symbols = malloc (sizeof (symbol *) * sm->height);
  for (int i = 0; i < sm->height; i++)
    sm->symbols[i] = malloc (sizeof (symbol) * sm->width);
}

static inline void
symbols_map_draw (smap *sm, double y, double x, symbol s)
{
  int ix = round (x);
  int iy = round (y);
  if (ix < 0 || iy < 0)
    return;
  if (ix >= sm->width || iy >= sm->height)
    return;
  sm->symbols[iy][ix] = s;
}

void
smap_free (smap *sm)
{
  for (int i = 0; i < sm->height; i++)
    free (sm->symbols[i]);
  free (sm->symbols);
}

static void
create_frame (u8buf *buf, int height, int width)
{
  for (int i = 0; i < height; i++)
    {
      u8str str = U8_STR_EMPTY;
      if (i == 0)
        {
          u8_str_append_str (&str, symbols[4]);
          u8_str_append_repeate_str (&str, symbols[3], width - 2);
          u8_str_append_str (&str, symbols[5]);
        }
      else if (i == height - 1)
        {
          u8_str_append_str (&str, symbols[6]);
          u8_str_append_repeate_str (&str, symbols[3], width - 2);
          u8_str_append_str (&str, symbols[7]);
        }
      else
        {
          u8_str_append_str (&str, symbols[2]);
          u8_str_append_repeate_str (&str, symbols[1], width - 2);
          u8_str_append_str (&str, symbols[2]);
        }
      u8_buffer_add_line (buf, str.chars, str.length);
    }
}

static symbol
get_corner (int broom, int bneighbor)
{
  if (EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return 8; // "╋";
  if (EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER))
    return 9; // "┣"
  if (EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, BOTTOM_BORDER))
    return 11; // "┳"
  if (EXPECT_BORDERS (broom, LEFT_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return 10; // "┫"
  if (EXPECT_BORDERS (broom, UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return 12; // "┻"
  if (EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && NOT_EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return 4; // "┏"
  if (NOT_EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return 7; // "┛"
  if (EXPECT_BORDERS (broom, UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER))
    return 6; // "┗"
  if (EXPECT_BORDERS (broom, LEFT_BORDER)
      && EXPECT_BORDERS (bneighbor, BOTTOM_BORDER))
    return 5; // "┓"
  if (EXPECT_BORDERS (broom, UPPER_BORDER)
      && NOT_EXPECT_BORDERS (bneighbor, RIGHT_BORDER))
    return 3; // "━"
  if (EXPECT_BORDERS (broom, LEFT_BORDER)
      && NOT_EXPECT_BORDERS (bneighbor, BOTTOM_BORDER))
    return 2; // "┃"

  return 1; // " "
}

void
draw_in_the_middle_of_room (smap *sm, int r, int c, symbol s)
{
  int y = r * laby_room_height + (laby_room_height / 2);
  int x = c * laby_room_width + (laby_room_width / 2);
  symbols_map_draw (sm, y, x, s);
}

/**
 * @y number of the room by vertical
 * @x number of the room by horizontal
 * @r number of the char of the room by vertical
 * @c number of the char of the room by horizontal
 */
static void
draw_room (smap *sm, Laby *lab, int y, int x, int r, int c)
{
  /* We will render left and upper borders at once.
   * To choose correct symbol for the corner we need to know a
   * neighbor. */
  int border = laby_get_borders (lab, y, x);
  int neighbor = laby_get_borders (lab, y - 1, x - 1);

  symbol *idx
      = &sm->symbols[r + y * laby_room_height][c + x * laby_room_width];

  /* render the first row of symbols of the room */
  if (r == 0)
    {
      *idx = (c == 0)                  ? get_corner (border, neighbor)
             : (border & UPPER_BORDER) ? 3
                                       : SIDX_EMPTY;
    }
  /* render the content of the room (the second row) */
  else
    {
      *idx = ((c == 0) && (border & LEFT_BORDER)) ? 2 : SIDX_EMPTY;
    }
}

void
draw_laby (smap *sm, Laby *lab)
{
  /* Render rooms from every row, plus one extra row for the bottom borders */
  for (int y = 0; y <= lab->rows; y++)
    {
      /* iterates over room height (only one line for the last extra row) */
      int n = (y < lab->rows) ? laby_room_height : 1;
      for (int r = 0; r < n; r++)
        {
          /* Take a room from the row, plus one more for the right border */
          for (int x = 0; x <= lab->cols; x++)
            {
              /* Iterate over columns of the single room
               * (only one symbol for the extra right room) */
              int k = (x < lab->cols) ? laby_room_width : 1;
              for (int c = 0; c < k; c++)
                draw_room (sm, lab, y, x, r, c);
            }
        }
    }
}

static inline void
draw_inside_borders (smap *sm, int y, int x, symbol s)
{
  if (x < 1 || y < 1)
    return;
  if (x >= (sm->width - 1) || y >= (sm->height - 1))
    return;
  sm->symbols[y][x] = s;
}

int
get_borders_lines (const Laby *lab, const int r, const int c, Line dest[4])
{
  Line dg;
  dg.p0.y = r * laby_room_height;
  dg.p0.x = c * laby_room_width;
  dg.p1.y = dg.p0.y + laby_room_height;
  dg.p1.x = dg.p0.x + laby_room_width;

  int borders = laby_get_borders (lab, r, c);

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
          dest[j].p0.y = dg.p1.y;
          dest[j].p0.x = dg.p0.x;
          dest[j].p1.y = dg.p1.y;
          dest[j].p1.x = dg.p1.x;
          break;
        case RIGHT_BORDER:
          dest[j].p0.y = dg.p0.y;
          dest[j].p0.x = dg.p1.x;
          dest[j].p1.y = dg.p1.y;
          dest[j].p1.x = dg.p1.x;
          break;
        case UPPER_BORDER:
          dest[j].p0.y = dg.p0.y;
          dest[j].p0.x = dg.p0.x;
          dest[j].p1.y = dg.p0.y;
          dest[j].p1.x = dg.p1.x;
          break;
        case LEFT_BORDER:;
          dest[j].p0.y = dg.p0.y;
          dest[j].p0.x = dg.p0.x;
          dest[j].p1.y = dg.p1.y;
          dest[j].p1.x = dg.p0.x;
          break;
        }
      j++;
    }
  return j;
}

static inline _Bool
is_equal (double a, double b, double c)
{
  return (fabs (a - b) > 1e-5) && (fabs (b - c) > 1e-5);
}

static _Bool
is_on_border (int y, int x, Line bls[], int count)
{
  /* here we can simplify checking using inside about lines */
  for (int i = 0; i < count; i++)
    {
      if (is_equal (bls[i].p0.x, bls[i].p0.x, x))
        return 1;
      if (is_equal (bls[i].p0.y, bls[i].p0.y, y))
        return 1;
    }
  return 0;
}

static _Bool
is_vector_intersects_with_borders (Laby *lab, int y0, int x0, int y, int x)
{
  /* skip the same points */
  if (y0 == y && x0 == x)
    return 0;

  int r0 = y0 / laby_room_height;
  int c0 = x0 / laby_room_width;
  int r = y / laby_room_height;
  int c = x / laby_room_width;

  Line bls[4];
  int bls_count = get_borders_lines (lab, r, c, bls);

  /* both points may be in the same room,
   * in this case check is x:y on a border is enough */
  if (r == r0 && c == c0)
    return is_on_border (y, x, bls, bls_count);

  /* Checks intersection with borders of the target room */
  for (int i = 0; i < bls_count; i++)
    {
      if (line_is_intersectedp (&bls[i], x0, y0, x, y))
        return 1;
    }
  /* Checks intersection with borders of the departure room */
  bls_count = get_borders_lines (lab, r0, c0, bls);
  for (int i = 0; i < bls_count; i++)
    {
      if (line_is_intersectedp (&bls[i], x0, y0, x, y))
        return 1;
    }

  return 0;
}

/**
 * @dx increment
 * @x the current coordinate
 * @x1 the target coordinate
 * @return 0 if the current coordinate is out of range
 */
static inline _Bool
is_in_range (double dx, double x, double x1)
{
  return isgreater (fabs (dx), 1e-5)
         && (isgreater (dx, 0) ? islessequal (x, x1) : isgreaterequal (x, x1));
}

/**
 * Moves from f0 to f1.
 */
static void
draw_visible_in_direction (smap *sm, Laby *lab, double fy0, double fx0,
                           double dy, double dx, double fy1, double fx1)
{
  int iy_prev = round (fy0);
  int ix_prev = round (fx0);
  int iy = iy_prev;
  int ix = ix_prev;
  while ((is_in_range (dy, fy0, fy1) || is_in_range (dx, fx0, fx1))
         && !is_vector_intersects_with_borders (lab, iy_prev, ix_prev, iy, ix))
    {
      iy_prev = iy;
      ix_prev = ix;
      draw_inside_borders (sm, iy, ix, SIDX_LIGHT);
      if (is_in_range (dy, fy0, fy1))
        {
          fy0 += dy;
          iy = round (fy0);
        }
      if (is_in_range (dx, fx0, fx1))
        {
          fx0 += dx;
          ix = round (fx0);
        }
    }
}

void
draw_visible_area (smap *sm, Laby *lab, int y, int x, int range)
{
  double l = 0;
  /* Than bigger denominator, than better result,
   * but more computations needed */
  double dl = M_PI / (range * range * 11);
  /* Coz the room has sides with different length, we should draw the visible
   * area not as a circle, but as an ellipse.
   * Also, we should care about different proportions of the symbols in the
   * terminal and use additional coefficient */
  double h = range;
  double w = 1.5 * ((double)laby_room_width / laby_room_height) * range;
  while (islessequal (l, M_PI_2))
    {
      /* Calculate deltas as sides of triangle with hypotenuse == 1 */
      double fdx = cos (l);
      double fdy = sin (l);
      /* Now, calculate a coordinates of the point on the border of the visible
       * ellipse area */
      double y1 = h * fdy;
      double x1 = w * fdx;
      /* Mark visible symbols in 4 directions: */
      draw_visible_in_direction (sm, lab, y, x, fdy, fdx, y + y1, x + x1);
      draw_visible_in_direction (sm, lab, y, x, -fdy, fdx, y - y1, x + x1);
      draw_visible_in_direction (sm, lab, y, x, fdy, -fdx, y + y1, x - x1);
      draw_visible_in_direction (sm, lab, y, x, -fdy, -fdx, y - y1, x - x1);
      l += dl;
    }
}

void
render_symbols_map (u8buf *dest, const smap *source)
{
  for (int i = 0; i < source->height; i++)
    {
      for (int j = 0; j < source->width; j++)
        {
          const char *s = symbols[source->symbols[i][j]];
          u8_buffer_append_str (dest, s, strlen (s));
        }
      u8_buffer_end_line (dest);
    }
}

void
render_game (u8buf *buf, Game *game)
{
  smap sm;
  int y = laby_room_height * game->player.row;
  int x = laby_room_width * game->player.col;
  smap_init (&sm, game->lab.rows, game->lab.cols, laby_room_height,
             laby_room_width);
  draw_laby (&sm, &game->lab);
  draw_visible_area (&sm, &game->lab, y, x, game->player.visible_range);
  draw_in_the_middle_of_room (&sm, game->exit.row, game->exit.col, SIDX_EXIT);
  draw_in_the_middle_of_room (&sm, game->player.row, game->player.col,
                              SIDX_PLAYER);
  render_symbols_map (buf, &sm);
  smap_free (&sm);
}

void
render_welcome_screen (u8buf *buf, void *menu)
{
  Menu *m = (Menu *)menu;
  /* Blink menu option */
  time_t now = time (NULL);
  if ((now - m->last_update_at) > 0.4)
    {
      // m->state = -m->state;
      m->last_update_at = now;
    }

  u8_buffer_parse (buf, WELCOME_SCREEN);
  u8buf label = U8_BUF_EMPTY;
  switch (m->state)
    {
    case 0:
      m->state = 1;
      break;
      /* New game */
    case 1:
      u8_buffer_parse (&label, LB_NEW_GAME);
      u8_buffer_merge (buf, &label, 14, 22);
      break;
      /* Exit */
    case 2:
      u8_buffer_parse (&label, LB_EXIT);
      u8_buffer_merge (buf, &label, 14, 30);
      break;
    }
  u8_buffer_free (&label);
}

void
render_pause_menu (u8buf *buf, void *menu)
{
  Menu *m = (Menu *)menu;
  u8buf frame = U8_BUF_EMPTY;
  u8buf label = U8_BUF_EMPTY;
  create_frame (&frame, 8, 40);
  switch (m->state)
    {
    case 1:
      u8_buffer_parse (&label, LB_CONTINUE);
      u8_buffer_merge (&frame, &label, 3, 4);
      break;
    case 0:
      u8_buffer_parse (&label, LB_EXIT);
      u8_buffer_merge (&frame, &label, 3, 14);
      break;
    }
  u8_buffer_merge (buf, &frame, 8, 19);
  u8_buffer_free (&frame);
  u8_buffer_free (&label);
}

void
render_winning (u8buf *buf, void *menu)
{
  u8buf frame = U8_BUF_EMPTY;
  u8buf label = U8_BUF_EMPTY;
  create_frame (&frame, 10, 60);
  u8_buffer_parse (&label, LB_YOU_WIN);
  u8_buffer_merge (&frame, &label, 2, 2);
  u8_buffer_merge (buf, &frame, 6, 8);
  u8_buffer_free (&frame);
  u8_buffer_free (&label);
}

void
render (Game *game)
{
  u8buf buf = U8_BUF_EMPTY;

  /* only the main screen overlaps the level completely,
   * in all other cases we should render the laby before anything else. */
  if (game->state != ST_MAIN_MENU)
    render_game (&buf, game);

  switch (game->state)
    {
    case ST_MAIN_MENU:
      render_welcome_screen (&buf, game->menu);
      break;
    case ST_PAUSE:
      render_pause_menu (&buf, game->menu);
      break;
    case ST_WIN:
      render_winning (&buf, game->menu);
      break;
    case ST_GAME:
      break;
    }

  int rowpad = (screen_rows - game_window_rows) / 2;
  rowpad = (rowpad > 0) ? rowpad : 0;
  int colpad = (screen_cols - game_window_cols) / 2;
  colpad = (colpad > 0) ? colpad : 0;

  int rows = (screen_rows < game_window_rows) ? screen_rows : game_window_rows;
  int cols = (screen_cols < game_window_cols) ? screen_cols : game_window_cols;

  u8_buffer_write (STDIN_FILENO, &buf, rowpad, colpad, rows, cols);
  u8_buffer_free (&buf);
}
