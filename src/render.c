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
#include "term.h"
#include "u8.h"

static const char *s_empty = " ";
static const char *s_player = "@";
static const char *s_exit = "⛿";
static const char *s_light = "·";

/* Symbols to render borders */
//       0    1    2    3    4    5    6    7    8    9    10
static const char *s_borders[]
    = { "┃", "━", "┏", "┓", "┗", "┛", "╋", "┣", "┫", "┳", "┻" };

Menu *
create_menu (enum game_state state)
{
  Menu *menu = malloc (sizeof (Menu));
  menu->option = 0;

  if (state == ST_MAIN_MENU)
    menu->option = M_NEW_GAME;

  if (state == ST_PAUSE)
    menu->option = M_CONTINUE;

  if (state == ST_CMD)
    menu->cmd = malloc (sizeof (char) * MAX_CMD_LENGTH);

  return menu;
}

void
close_menu (Menu *menu, enum game_state state)
{
  if (state == ST_CMD)
    free (menu->cmd);

  free (menu);
}

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
render_room (Render *render, Laby *lab, int r, int c, int y, int x)
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

      enum content ct = (x == render->laby_room_width / 2)
                            ? laby_get_content (lab, r, c)
                            : 0;

      s = (draw_border)                   ? s_borders[0]
          : (!is_known_room)              ? s_empty
          : (ct == C_PLAYER)              ? s_player
          : (ct == C_EXIT)                ? s_exit
          : (laby_is_visible (lab, r, c)) ? s_light
                                          : s_empty;
    }
  u8_buffer_append_str (&render->buf, s, strlen (s));
}

void
render_laby (Render *render, Laby *lab)
{
  int rows = render->visible_rows + render->visible_rows_pad;
  rows = (rows < lab->rows) ? rows : lab->rows;
  int cols = render->visible_cols + render->visible_cols_pad;
  cols = (cols < lab->cols) ? cols : lab->cols;

  /* Render rooms from every row, plus one extra row for the bottom borders */
  for (int r = render->visible_rows_pad; r <= rows; r++)
    {
      /* iterates over room height (only one line for the last extra row) */
      int rh = (r < rows) ? render->laby_room_height : 1;
      for (int ry = 0; ry < rh; ry++)
        {
          /* Take a room from the row, plus one more for the right border */
          int c = render->visible_cols_pad;
          for (; c <= cols; c++)
            {
              /* Iterate over columns of the single room
               * (only one symbol for the extra right room) */
              int rw = (c < cols) ? render->laby_room_width : 1;
              for (int rx = 0; rx < rw; rx++)
                render_room (render, lab, r, c, ry, rx);
            }
          u8_buffer_end_line (&render->buf);
        }
    }
}

void
render_update_visible_area (Render *render, Player *player, int laby_rows,
                            int laby_cols)
{
  int margin = player->visible_range;
  /* if player is near the top border of the visible area */
  if (player->row <= render->visible_rows_pad + margin)
    {
      render->visible_rows_pad = player->row - margin;
      render->visible_rows_pad
          = (0 < render->visible_rows_pad) ? render->visible_rows_pad : 0;
    }

  /* if player is near the left border of the visible area */
  if (player->col <= render->visible_cols_pad + margin)
    {
      render->visible_cols_pad = player->col - margin;
      render->visible_cols_pad
          = (0 < render->visible_cols_pad) ? render->visible_cols_pad : 0;
    }

  /* if player is near the bottom of the visible area, but far from the laby's
   * border */
  if (player->row >= (render->visible_rows_pad + render->visible_rows - margin)
      && (laby_rows - player->row) >= margin)
    render->visible_rows_pad = player->row - render->visible_rows + margin;

  /* if player is near the right border of the visible area, but far from the
   * laby's border  */
  if (player->col >= (render->visible_cols_pad + render->visible_cols - margin)
      && (laby_cols - player->col) >= margin)
    render->visible_cols_pad = player->col - render->visible_cols + margin;
}

void
render_welcome_screen (Render *render, Menu *menu)
{
  u8_buffer_parse (&render->buf, WELCOME_SCREEN);
  u8buf label = U8_BUF_EMPTY;
  switch (menu->option)
    {
      /* New game */
    case M_NEW_GAME:
      u8_buffer_parse (&label, LB_NEW_GAME);
      u8_buffer_merge (&render->buf, &label, 14, 22);
      break;
      /* Exit */
    case M_EXIT:
      u8_buffer_parse (&label, LB_EXIT);
      u8_buffer_merge (&render->buf, &label, 14, 30);
      break;
    default:
      break;
    }
  u8_buffer_free (&label);
}

void
render_pause_menu (Render *render, Menu *menu)
{
  u8buf frame = U8_BUF_EMPTY;
  u8buf label = U8_BUF_EMPTY;
  create_frame (&frame, 8, 40);
  switch (menu->option)
    {
    case M_CONTINUE:
      u8_buffer_parse (&label, LB_CONTINUE);
      u8_buffer_merge (&frame, &label, 3, 4);
      break;
    case M_EXIT:
      u8_buffer_parse (&label, LB_EXIT);
      u8_buffer_merge (&frame, &label, 3, 14);
      break;
    default:
      break;
    }
  u8_buffer_merge (&render->buf, &frame, 8, 19);
  u8_buffer_free (&frame);
  u8_buffer_free (&label);
}

void
render_winning (Render *render, Game *game)
{
  u8_buffer_clean (&render->buf);
  laby_mark_whole_as_known (&L);
  render_laby (render, &L);

  u8buf frame = U8_BUF_EMPTY;
  u8buf label = U8_BUF_EMPTY;
  create_frame (&frame, 10, 60);
  u8_buffer_parse (&label, LB_YOU_WIN);
  u8_buffer_merge (&frame, &label, 2, 2);
  u8_buffer_merge (&render->buf, &frame, 6, 8);
  u8_buffer_free (&frame);
  u8_buffer_free (&label);
}

static void
render_cmd (Render *render, char *cmd, int len)
{
  u8str cmd_prompt;
  u8_str_init (&cmd_prompt, ": ", 2);
  u8_str_append (&cmd_prompt, cmd, len);
  u8_str_append_repeate (&cmd_prompt, " ", 1,
                         render->game_screen_width - len - 2);
  u8_buffer_replace_str (&render->buf, render->buf.lines_count - 1,
                         &cmd_prompt);
}

void
render (Render *render, Game *game)
{
  /* actual visible height */
  int visible_height = (terminal_window_height < render->game_screen_height)
                           ? terminal_window_height
                           : render->game_screen_height;
  /* actual visible width */
  int visible_width = (terminal_window_width < render->game_screen_width)
                          ? terminal_window_width
                          : render->game_screen_width;

  if (visible_height < render->game_screen_height
      || visible_width < render->game_screen_width)
    {
      char msg[60];
      int len = sprintf (
          msg, CUP "The minimal size of the terminal window is %dx%d",
          render->game_screen_height, render->game_screen_width);
      write (STDIN_FILENO, msg, len);
      len = sprintf (msg, "\nbut it has %dx%d", terminal_window_height,
                     terminal_window_width);
      write (STDIN_FILENO, msg, len);
      return;
    }

  u8_buffer_clean (&render->buf);

  /* only the main screen overlaps the level completely,
   * in all other cases we should render the laby before anything else. */
  if (game->state != ST_MAIN_MENU)
    {
      render_update_visible_area (render, &P, L.rows, L.cols);
      render_laby (render, &game->lab);
    }

  switch (game->state)
    {
    case ST_MAIN_MENU:
      render_welcome_screen (render, game->menu);
      break;
    case ST_PAUSE:
      render_pause_menu (render, game->menu);
      break;
    case ST_WIN:
      render_winning (render, game);
      break;
    case ST_CMD:
      render_cmd (render, M->cmd, M->option);
      break;
    case ST_GAME:
      /* Everything already rendered */
      break;
    }

  /* padding of the visible game screen and terminal window */
  int screen_y_pad = (terminal_window_height - render->game_screen_height) / 2;
  screen_y_pad = (screen_y_pad > 0) ? screen_y_pad : 0;
  int screen_x_pad = (terminal_window_width - render->game_screen_width) / 2;
  screen_x_pad = (screen_x_pad > 0) ? screen_x_pad : 0;
  //
  // if (L.rows < render->visible_rows || L.cols < render->visible_cols)
  //   write (STDIN_FILENO, ED_FULL, 4);

  u8_buffer_write (STDIN_FILENO, &render->buf, screen_y_pad, screen_x_pad,
                   render->game_screen_height, render->game_screen_width);
  u8_buffer_free (&render->buf);
}
