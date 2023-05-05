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
#include "dbuf.h"
#include "game.h"
#include "term.h"

typedef struct
{
  /* last update time */
  time_t lut;
  int state;
  dbuf frame;
} screen;

#define SCREEN_EMPTY                                                          \
  {                                                                           \
    0.0, 0, 0, DBUF_EMPTY                                                     \
  }

/* The count of symbols by vertical of one room.  */
static const int laby_room_rows = 2;

/* The count of symbols by horizontal of one room.  */
static const int laby_room_cols = 4;

/* windows resolution in chars */
int screen_rows = 0;
int screen_cols = 0;

static int
expect_borders (int border, char expected)
{
  return (border & expected) == expected;
}

static int
not_expect_borders (int border, char expected)
{
  return (border & expected) == 0;
}

static char *
get_corner (int border, int neighbor)
{
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && not_expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┏";
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "╋";
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER))
    return "┣";
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && expect_borders (neighbor, BOTTOM_BORDER))
    return "┳";
  if (expect_borders (border, LEFT_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┫";
  if (expect_borders (border, UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┻";
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && not_expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┏";
  if (not_expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┛";
  if (expect_borders (border, UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER))
    return "┗";
  if (expect_borders (border, LEFT_BORDER)
      && expect_borders (neighbor, BOTTOM_BORDER))
    return "┓";
  if (expect_borders (border, UPPER_BORDER)
      && not_expect_borders (neighbor, RIGHT_BORDER))
    return "━";
  if (expect_borders (border, LEFT_BORDER)
      && not_expect_borders (neighbor, BOTTOM_BORDER))
    return "┃";

  return " ";
}

static char *
get_creature (level *level, int y, int x)
{
  if (level->player.y == y && level->player.x == x)
    return "@";
  else
    return 0;
}

static void
render_room (level *level, int y, int x, int r, int c, dbuf *buf)
{
  laby *lab = &level->lab;

  /* We will render left and upper borders at once.
   * To choose correct symbol for the corner we need to know a
   * neighbor. */
  int border = laby_get_border (lab, y, x);
  int neighbor = laby_get_border (lab, y - 1, x - 1);

  int is_visited = laby_is_visited (lab, y, x);
  char *creature = get_creature (level, y, x);

  char *s;
  /* render the first row of symbols of the room */
  if (r == 0)
    {
      s = (c == 0)                  ? get_corner (border, neighbor)
          : (border & UPPER_BORDER) ? "━"
          : (is_visited || laby_is_visited (lab, y - 1, x)) ? "·"
                                                            : " ";
    }
  /* render the content of the room (the second row) */
  else
    {
      int is_border = (c == 0) && (border & LEFT_BORDER);

      s = (is_border)                       ? "┃"
          : (creature && (r > 0 && c == 2)) ? creature
          : (is_visited)                    ? "·"
                                            : " ";
    }
  buffer_append_str (buf, s, strlen (s));
}

/**
 * Renders the labyrinth `lab` to the buffer `buf`.
 */
void
render_level (level *level, dbuf *buf)
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
            buffer_end_line (buf);
        }
    }
}

static void
render_welcome_screen (screen *s, dbuf *buf)
{
  int rowpad = (screen_rows - WELCOME_SCREEN_ROWS) / 2;
  rowpad = (rowpad < 0) ? 0 : rowpad;
  int colpad = (screen_cols - WELCOME_SCREEN_COLS) / 2;
  colpad = (colpad < 0) ? 0 : colpad;

  buffer_merge (buf, &s->frame, rowpad, colpad);

  /* Blink menu option */
  time_t now = time (NULL);
  if ((now - s->lut) > 0.7)
    {
      s->state ^= 1;
      s->lut = now;
    }

  if (s->state)
    {
      dbuf label;
      buffer_parse (&label, LB_NEW_GAME);
      buffer_merge (buf, &label, rowpad + 12, colpad + 24);
      buffer_free (&label);
    }
}

void
render (game *game)
{
  /* Put the cursor to the upper left corner */
  dbuf buf;
  buffer_init (&buf, CUP);
  switch (game->state)
    {
    case ST_MAIN_MENU:
      render_welcome_screen ((screen *)game->menu, &buf);
      break;
    default:
      render_level (&game->level, &buf);
    }
  buffer_write (STDIN_FILENO, &buf);
  buffer_free (&buf);
}

void *
create_menu (const game *game, enum game_state state)
{
  // switch (state)
  //   {
  //   case ST_MAIN_MENU:

  screen *welcome_screen = malloc (sizeof (screen));
  welcome_screen->state = 1;
  buffer_parse (&welcome_screen->frame, WELCOME_SCREEN);
  return welcome_screen;
  // }
}

void
close_menu (void *menu, enum game_state state)
{
  screen *welcome_screen = (screen *)menu;
  buffer_free (&welcome_screen->frame);
  free (welcome_screen);
}

enum command
read_command (game *game)
{
  key_p key = read_key ();
  switch (game->state)
    {
    case ST_MAIN_MENU:
      if (key.len == 1 && key.chars[0] == ENTER_KEY)
        return CMD_NEW_GAME;
      if (key.len == 1 && key.chars[0] == ESC)
        return CMD_EXIT;

    case ST_GAME:
      if (key.len == 1 && key.chars[0] == ESC)
        return CMD_EXIT;
      if (key.len == 3 && key.chars[1] == '[')
        {
          switch (key.chars[2])
            {
            case 'A':
              return CMD_MV_UP;
            case 'B':
              return CMD_MV_DOWN;
            case 'C':
              return CMD_MV_RIGHT;
            case 'D':
              return CMD_MV_LEFT;
            }
        }

    default:
      return CMD_NOTHING;
    }
}
