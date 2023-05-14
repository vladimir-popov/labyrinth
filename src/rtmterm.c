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

typedef struct
{
  time_t last_update_at;
  int state;
} menu;

#define MENU_EMPTY                                                            \
  {                                                                           \
    0.0, 0                                                                    \
  }

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

enum key
read_key ()
{
  key_p kp = read_pressed_key ();
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

static char *
get_corner (int border, int neighbor, char *no_corner)
{
  if (EXPECT_BORDERS (border, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "╋";
  if (EXPECT_BORDERS (border, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (neighbor, RIGHT_BORDER))
    return "┣";
  if (EXPECT_BORDERS (border, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (neighbor, BOTTOM_BORDER))
    return "┳";
  if (EXPECT_BORDERS (border, LEFT_BORDER)
      && EXPECT_BORDERS (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┫";
  if (EXPECT_BORDERS (border, UPPER_BORDER)
      && EXPECT_BORDERS (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┻";
  if (EXPECT_BORDERS (border, LEFT_BORDER | UPPER_BORDER)
      && NOT_EXPECT_BORDERS (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┏";
  if (NOT_EXPECT_BORDERS (border, LEFT_BORDER | UPPER_BORDER)
      && EXPECT_BORDERS (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┛";
  if (EXPECT_BORDERS (border, UPPER_BORDER)
      && EXPECT_BORDERS (neighbor, RIGHT_BORDER))
    return "┗";
  if (EXPECT_BORDERS (border, LEFT_BORDER)
      && EXPECT_BORDERS (neighbor, BOTTOM_BORDER))
    return "┓";
  if (EXPECT_BORDERS (border, UPPER_BORDER)
      && NOT_EXPECT_BORDERS (neighbor, RIGHT_BORDER))
    return "━";
  if (EXPECT_BORDERS (border, LEFT_BORDER)
      && NOT_EXPECT_BORDERS (neighbor, BOTTOM_BORDER))
    return "┃";

  return no_corner;
}

static char *
get_object (level *level, int y, int x)
{
  if (level->player.y == y && level->player.x == x)
    return "@";
  if (level->exit.y == y && level->exit.x == x)
    return "⛿";
  else
    return 0;
}

/**
 * @y number of the room by vertical
 * @x number of the room by horizontal
 * @r number of the char by vertical
 * @c number of the char by horizontal
 */
static void
render_room (level *level, int y, int x, int r, int c, u8buf *buf)
{
  laby *lab = &level->lab;

  /* We will render left and upper borders at once.
   * To choose correct symbol for the corner we need to know a
   * neighbor. */
  int border = laby_get_border (lab, y, x);
  int neighbor = laby_get_border (lab, y - 1, x - 1);

  char *object = get_object (level, y, x);

  char *s;
  /* render the first row of symbols of the room */
  if (r == 0)
    {
      s = (laby_is_visible (lab, y - 1, x)) ? "·" : " ";
      s = (c == 0)                  ? get_corner (border, neighbor, s)
          : (border & UPPER_BORDER) ? "━"
                                    : s;
    }
  /* render the content of the room (the second row) */
  else
    {
      int is_border = (c == 0) && (border & LEFT_BORDER);

      s = (is_border)                     ? "┃"
          : (object && (r > 0 && c == 2)) ? object
          : (laby_is_visible (lab, y, x)) ? "·"
                                          : " ";
    }
  u8_buffer_append_str (buf, s, strlen (s));
}

/**
 * Renders the labyrinth `lab` to the buffer `buf`.
 */
void
render_level (level *level, u8buf *buf)
{
  laby *lab = &level->lab;

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
                {
                  render_room (level, y, x, r, c, buf);
                }
            }
          if (y < lab->height)
            u8_buffer_end_line (buf);
        }
    }
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
  art_border border = ART_SINGLE_BORDER;
  art_create_frame (&frame, 8, 40, border);
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
  art_border border = ART_SINGLE_BORDER;
  art_create_frame (&frame, 10, 60, border);
  u8_buffer_parse (&label, LB_YOU_WIN);
  u8_buffer_merge (&frame, &label, 2, 2);
  u8_buffer_merge (buf, &frame, 6, 8);
  u8_buffer_free (&frame);
  u8_buffer_free (&label);
}

void
render (game *game)
{
  /* Put the cursor to the upper left corner */
  u8buf buf = U8_BUF_EMPTY;
  if (game->state != ST_MAIN_MENU)
    render_level (&game->level, &buf);
  switch (game->state)
    {
    case ST_MAIN_MENU:
      render_welcome_screen ((menu *)game->menu, &buf);
      break;
    case ST_PAUSE:
      render_pause_menu ((menu *)game->menu, &buf);
      break;
    case ST_WIN:
      render_winning ((menu *)game->menu, &buf);
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

  u8_buffer_write (STDIN_FILENO, &buf, rowpad, colpad, rows, cols);
  u8_buffer_free (&buf);
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
