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

static const char *s_empty = " ";
static const char *s_player = "@";
static const char *s_exit = "⛿";
static const char *s_light = "·";

/* Symbols to render borders */
//       0    1    2    3    4    5    6    7    8    9    10
static const char *s_borders[]
    = { "┃", "━", "┏", "┓", "┗", "┛", "╋", "┣", "┫", "┳", "┻" };

static void
create_frame (u8buf *buf, int height, int width)
{
  for (int i = 0; i < height; i++)
    {
      u8str str = U8_STR_EMPTY;
      if (i == 0)
        {
          u8_str_append_str (&str, s_borders[2]);
          u8_str_append_repeate_str (&str, s_borders[1], width - 2);
          u8_str_append_str (&str, s_borders[3]);
        }
      else if (i == height - 1)
        {
          u8_str_append_str (&str, s_borders[4]);
          u8_str_append_repeate_str (&str, s_borders[1], width - 2);
          u8_str_append_str (&str, s_borders[5]);
        }
      else
        {
          u8_str_append_str (&str, s_borders[0]);
          u8_str_append_repeate_str (&str, " ", width - 2);
          u8_str_append_str (&str, s_borders[0]);
        }
      u8_buffer_add_line (buf, str.chars, str.length);
    }
}

static const char *
get_top_left_corner (Laby *lab, int r, int c)
{
  _Bool is_known_room = laby_is_known_room (lab, r, c)
                        || laby_is_known_room (lab, r - 1, c)
                        || laby_is_known_room (lab, r - 1, c - 1)
                        || laby_is_known_room (lab, r, c - 1);
  if (!is_known_room)
    return 0;

  /* We will render left and upper borders at once.
   * To choose correct symbol for the corner we need to know a
   * neighbor. */
  int broom = laby_get_borders (lab, r, c);
  int bneighbor = laby_get_borders (lab, r - 1, c - 1);

  if (EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return s_borders[6]; // "╋";
  if (EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER))
    return s_borders[7]; // "┣"
  if (EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, BOTTOM_BORDER))
    return s_borders[9]; // "┳"
  if (EXPECT_BORDERS (broom, LEFT_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return s_borders[8]; // "┫"
  if (EXPECT_BORDERS (broom, UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return s_borders[10]; // "┻"
  if (EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && NOT_EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return s_borders[2]; // "┏"
  if (NOT_EXPECT_BORDERS (broom, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return s_borders[5]; // "┛"
  if (EXPECT_BORDERS (broom, UPPER_BORDER)
      && EXPECT_BORDERS (bneighbor, RIGHT_BORDER))
    return s_borders[4]; // "┗"
  if (EXPECT_BORDERS (broom, LEFT_BORDER)
      && EXPECT_BORDERS (bneighbor, BOTTOM_BORDER))
    return s_borders[3]; // "┓"
  if (EXPECT_BORDERS (broom, UPPER_BORDER)
      && NOT_EXPECT_BORDERS (bneighbor, RIGHT_BORDER))
    return s_borders[1]; // "━"
  if (EXPECT_BORDERS (broom, LEFT_BORDER)
      && NOT_EXPECT_BORDERS (bneighbor, BOTTOM_BORDER))
    return s_borders[0]; // "┃"

  return 0;
}

/**
 * @r number of the room by vertical
 * @c number of the room by horizontal
 * @y number of the char inside the room by vertical
 * @x number of the char inside the room by horizontal
 */
static void
render_room (u8buf *buf, Laby *lab, int r, int c, int y, int x)
{
  int border = laby_get_borders (lab, r, c);
  _Bool is_known_room = laby_is_known_room (lab, r, c);

  /* render the first row of symbols of the room */
  const char *s;
  int draw_border = 0;
  if (y == 0)
    {
      draw_border = (border & UPPER_BORDER)
                    && (is_known_room || laby_is_known_room (lab, r - 1, c));
      s = (x == 0) ? get_top_left_corner (lab, r, c) : 0;
      s = (s)                                 ? s
          : (draw_border)                     ? s_borders[1]
          : (!is_known_room)                  ? s_empty
          : (laby_is_visible (lab, r, c))     ? s_light
          : (laby_is_visible (lab, r - 1, c)) ? s_light
                                              : s_empty;
    }
  /* render the content of the room (the second row) */
  else
    {
      draw_border = (x == 0) && (border & LEFT_BORDER)
                    && (is_known_room || laby_is_known_room (lab, r, c - 1));
      enum content ct
          = (x == laby_room_width / 2) ? laby_get_content (lab, r, c) : 0;

      s = (draw_border)                   ? s_borders[0]
          : (!is_known_room)              ? s_empty
          : (ct == C_PLAYER)              ? s_player
          : (ct == C_EXIT)                ? s_exit
          : (laby_is_visible (lab, r, c)) ? s_light
                                          : s_empty;
    }
  u8_buffer_append_str (buf, s, strlen (s));
}

void
render_laby (u8buf *buf, Laby *lab)
{
  /* Render rooms from every row, plus one extra row for the bottom borders */
  for (int r = 0; r <= lab->rows; r++)
    {
      /* iterates over room height (only one line for the last extra row) */
      int rh = (r < lab->rows) ? laby_room_height : 1;
      for (int ry = 0; ry < rh; ry++)
        {
          /* Take a room from the row, plus one more for the right border */
          for (int c = 0; c <= lab->cols; c++)
            {
              /* Iterate over columns of the single room
               * (only one symbol for the extra right room) */
              int rw = (c < lab->cols) ? laby_room_width : 1;
              for (int rx = 0; rx < rw; rx++)
                render_room (buf, lab, r, c, ry, rx);
            }
          u8_buffer_end_line (buf);
        }
    }
}

void
render_game (u8buf *buf, Game *game)
{
  render_laby (buf, &game->lab);
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
render_winning (u8buf *buf, Game *game)
{
  u8_buffer_clean (buf);
  laby_mark_whole_as_known (&L);
  render_laby (buf, &L);

  u8buf frame = U8_BUF_EMPTY;
  u8buf label = U8_BUF_EMPTY;
  create_frame (&frame, 10, 60);
  u8_buffer_parse (&label, LB_YOU_WIN);
  u8_buffer_merge (&frame, &label, 2, 2);
  u8_buffer_merge (buf, &frame, 6, 8);
  u8_buffer_free (&frame);
  u8_buffer_free (&label);
}

static inline _Bool
is_player_on_screen (Player *p, int height, int width)
{
  return PLAYER_Y (p->row) < height && PLAYER_X (p->col) < width;
}

static inline _Bool
is_visible_enough (Game *game, int visible_height, int visible_width)
{
  return visible_height >= game->laby_rows * laby_room_height
         && visible_width >= game->laby_cols * laby_room_width;
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
      render_winning (&buf, game);
      break;
    case ST_GAME:
      break;
    }

  /* padding of the visible game screen and terminal window */
  int screen_y_pad = (screen_height - game_window_height) / 2;
  screen_y_pad = (screen_y_pad > 0) ? screen_y_pad : 0;
  int screen_x_pad = (screen_width - game_window_width) / 2;
  screen_x_pad = (screen_x_pad > 0) ? screen_x_pad : 0;

  /* actual visible height */
  int visible_height = (screen_height < game_window_height)
                           ? screen_height
                           : game_window_height;
  /* actual visible width */
  int visible_width
      = (screen_width < game_window_width) ? screen_width : game_window_width;

  /* the minimal distance between player and a screen border */
  int player_y_pad = laby_room_height * 3;
  int player_x_pad = laby_room_width * 3;

  /* we should take only visible part of the buffer, which depends on the
   * player position */
  if (game->state != ST_MAIN_MENU
      && !is_visible_enough (game, visible_height, visible_width)
      && !is_player_on_screen (&game->player, visible_height, visible_width))
    {
      int buf_y_pad
          = PLAYER_Y (game->player.row) - visible_height + player_y_pad;
      buf_y_pad = (buf_y_pad < 0) ? 0 : buf_y_pad;

      int buf_x_pad
          = PLAYER_X (game->player.col) - visible_width + player_x_pad;
      buf_x_pad = (buf_x_pad < 0) ? 0 : buf_x_pad;

      u8_buffer_crop (&buf, buf_y_pad, buf_x_pad, visible_height,
                      visible_width);
    }

  u8_buffer_write (STDIN_FILENO, &buf, screen_y_pad, screen_x_pad,
                   visible_height, visible_width);
  u8_buffer_free (&buf);
}
