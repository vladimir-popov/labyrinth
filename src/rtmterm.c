/**
 *  This is terminal runtime for the game.
 *  Here we have implementations of all runtime-depended
 *  functions of the game, such as rendering or handling key pressing.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "art.h"
#include "game.h"
#include "term.h"
#include "u8.h"

/**
 * We will render a game to the two dimension byte buffer in terms of indexes
 * of the `symbols` array to avoid issue with different size of symbols.
 */
typedef unsigned char sidx;

#define SIDX_NOTHING 0
#define SIDX_EMPTY 1
#define SIDX_PLAYER 13
#define SIDX_EXIT 14
#define SIDX_LIGHT 15

// clang-format off
static const char *symbols[] = 
//   0    1    2    3    4    5    6    7    8    9
  { 
     "", " ", "┃", "━", "┏", "┓", "┗", "┛", "╋", "┣",
    "┫", "┳", "┻", "@", "⛿", "·" 
  };
// clang-format on

/**
 * Two dimensions array with indexes of the symbols.
 */
typedef struct
{
  int height;
  int width;
  sidx **index;
} bbuf;

#define BBUF_EMPTY                                                            \
  {                                                                           \
    0, 0, NULL                                                                \
  }

typedef struct
{
  time_t last_update_at;
  int state;
} menu;

#define MENU_EMPTY                                                            \
  {                                                                           \
    0.0, 0                                                                    \
  }

enum key
{
  KEY_UNKNOWN,
  KEY_ENTER,
  KEY_CANCEL,
  KEY_LEFT,
  KEY_UP,
  KEY_RIGHT,
  KEY_DOWN
};

/* The count of symbols by vertical of one room.  */
static const int laby_room_rows = 2;

/* The count of symbols by horizontal of one room.  */
static const int laby_room_cols = 4;

/* The count of visible chars by vertical. */
static const int game_rows = 25;
/* The count of visible chars by horizontal. */
static const int game_cols = 78;

/* The windows resolution in chars by vertical. */
int screen_rows = 0;
/* The windows resolution in chars by horizontal. */
int screen_cols = 0;

void
bbuf_init (bbuf *bb, int y_rooms, int x_rooms, int room_rows, int room_cols)
{
  bb->height = y_rooms * room_rows + 1;
  bb->width = x_rooms * room_cols + 1;
  bb->index = malloc (sizeof (sidx *) * bb->height);
  for (int i = 0; i < bb->height; i++)
    bb->index[i] = malloc (sizeof (sidx) * bb->width);
}

void
bbuf_free (bbuf *buf)
{
  for (int i = 0; i < buf->height; i++)
    free (buf->index[i]);
  free (buf->index);
}

void
bbuf_to_u8buf (const bbuf *source, u8buf *dest)
{
  for (int i = 0; i < source->height; i++)
    {
      for (int j = 0; j < source->width; j++)
        {
          const char *s = symbols[source->index[i][j]];
          u8_buffer_append_str (dest, s, strlen (s));
        }
      u8_buffer_end_line (dest);
    }
}

enum key
read_key ()
{
  key_code kp = read_pressed_key ();
  if (kp.len == 1)
    switch (kp.chars[0])
      {
      case ESC:
        return KEY_CANCEL;
      case KEY_CODE_ENTER:
        return KEY_ENTER;
      case 'j':
        return KEY_DOWN;
      case 'k':
        return KEY_UP;
      case 'h':
        return KEY_LEFT;
      case 'l':
        return KEY_RIGHT;
      }

  if (kp.len == 2)
    return KEY_UNKNOWN;

  if (kp.len == 3)
    switch (kp.chars[2])
      {
      case KEY_CODE_DOWN:
        return KEY_DOWN;
      case KEY_CODE_UP:
        return KEY_UP;
      case KEY_CODE_LEFT:
        return KEY_LEFT;
      case KEY_CODE_RIGHT:
        return KEY_RIGHT;
      }

  return KEY_UNKNOWN;
}

enum command
read_command (game *game)
{
  enum key key = read_key ();
  menu *m = (menu *)game->menu;
  switch (game->state)
    {
    case ST_MAIN_MENU:
      {
        switch (key)
          {
          case KEY_ENTER:
            return (m->state == 1) ? CMD_NEW_GAME : CMD_EXIT;
          case KEY_CANCEL:
            return CMD_EXIT;
          case KEY_UP:
            m->state = (m->state == 2) ? 1 : m->state;
            return CMD_NOTHING;

          case KEY_DOWN:
            m->state = (m->state == 1) ? 2 : m->state;
            return CMD_NOTHING;

          default:
            return CMD_NOTHING;
          }
      }

    case ST_GAME:
      switch (key)
        {
        case KEY_UP:
          return CMD_MV_UP;
        case KEY_DOWN:
          return CMD_MV_DOWN;
        case KEY_RIGHT:
          return CMD_MV_RIGHT;
        case KEY_LEFT:
          return CMD_MV_LEFT;
        case KEY_CANCEL:
          return CMD_PAUSE;
        default:
          return CMD_NOTHING;
        }

    case ST_PAUSE:
      switch (key)
        {
        case KEY_UP:
        case KEY_DOWN:
          m->state ^= 1;
          break;

        case KEY_CANCEL:
          return CMD_CONTINUE;

        case KEY_ENTER:
          if (m->state)
            return CMD_CONTINUE;
          else
            return CMD_EXIT;

        default:
          return CMD_NOTHING;
        }

    case ST_WIN:
      switch (key)
        {
        case KEY_CANCEL:
        case KEY_ENTER:
          return CMD_EXIT;
        default:
          return CMD_NOTHING;
        }
    }
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

static sidx
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

/**
 * @y number of the room by vertical
 * @x number of the room by horizontal
 * @r number of the char of the room by vertical
 * @c number of the char of the room by horizontal
 */
static void
prepare_room (laby *lab, int y, int x, int r, int c, bbuf *bb)
{
  /* We will render left and upper borders at once.
   * To choose correct symbol for the corner we need to know a
   * neighbor. */
  int border = laby_get_border (lab, y, x);
  int neighbor = laby_get_border (lab, y - 1, x - 1);

  sidx *idx = &bb->index[r + y * laby_room_rows][c + x * laby_room_cols];

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

/**
 * Prepare the labyrinth `lab` to render it.
 */
void
prepare_laby (laby *lab, bbuf *buf)
{
  /* Render rooms from every row, plus one extra row for the bottom borders */
  for (int y = 0; y <= lab->height; y++)
    {
      /* iterates over room height (only one line for the last extra row) */
      int n = (y < lab->height) ? laby_room_rows : 1;
      for (int r = 0; r < n; r++)
        {
          /* Take a room from the row, plus one more for the right border */
          for (int x = 0; x <= lab->width; x++)
            {

              /* Iterate over columns of the single room
               * (only one symbol for the extra right room) */
              int k = (x < lab->width) ? laby_room_cols : 1;
              for (int c = 0; c < k; c++)
                prepare_room (lab, y, x, r, c, buf);
            }
        }
    }
}

static void
put_to_room (bbuf *bb, int y, int x, sidx idx)
{
  int i = y * laby_room_rows + (laby_room_rows / 2);
  int j = x * laby_room_cols + (laby_room_cols / 2);
  bb->index[i][j] = idx;
}

/**
 * Additional preparation of the labyrinth this method renders visibility and
 * all objects and creatures.
 */
void
prepare_level (level *level, bbuf *buf)
{
  prepare_laby (&level->lab, buf);
  put_to_room (buf, level->exit.y, level->exit.x, SIDX_EXIT);
  put_to_room (buf, level->player.y, level->player.x, SIDX_PLAYER);
}

void
render_welcome_screen (menu *m, u8buf *buf)
{
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
render_pause_menu (menu *m, u8buf *buf)
{
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
render_winning (menu *m, u8buf *buf)
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
render (game *game)
{
  u8buf ubuf = U8_BUF_EMPTY;
  bbuf bbuf = BBUF_EMPTY;
  bbuf_init (&bbuf, game->height, game->width, laby_room_rows, laby_room_cols);

  /* only the main screen overlaps the level completely,
   * in all other cases we should render the laby before anything else. */
  if (game->state != ST_MAIN_MENU)
    {
      prepare_level (&game->level, &bbuf);
      bbuf_to_u8buf (&bbuf, &ubuf);
    }

  switch (game->state)
    {
    case ST_MAIN_MENU:
      render_welcome_screen ((menu *)game->menu, &ubuf);
      break;
    case ST_PAUSE:
      render_pause_menu ((menu *)game->menu, &ubuf);
      break;
    case ST_WIN:
      render_winning ((menu *)game->menu, &ubuf);
      break;
    case ST_GAME:
      break;
    }

  int rowpad = (screen_rows - game_rows) / 2;
  rowpad = (rowpad > 0) ? rowpad : 0;
  int colpad = (screen_cols - game_cols) / 2;
  colpad = (colpad > 0) ? colpad : 0;

  int rows = (screen_rows < game_rows) ? screen_rows : game_rows;
  int cols = (screen_cols < game_cols) ? screen_cols : game_cols;

  u8_buffer_write (STDIN_FILENO, &ubuf, rowpad, colpad, rows, cols);
  u8_buffer_free (&ubuf);
}

void *
create_menu (const game *game, enum game_state state)
{
  menu *m = malloc (sizeof (menu));
  m->state = 0;
  return m;
}

void
close_menu (void *menu, enum game_state state)
{
  free (menu);
}
