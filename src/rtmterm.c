#include <stdlib.h>
#include "game.h"
#include "render.h"
#include "term.h"

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
  Key_Code kp = read_pressed_key ();
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
read_command (Game *game)
{
  enum key key = read_key ();
  Menu *m = (Menu *)game->menu;
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

void *
create_menu (const Game *game, enum game_state state)
{
  Menu *m = malloc (sizeof (Menu));
  m->state = 0;
  return m;
}

void
close_menu (void *menu, enum game_state state)
{
  free (menu);
}

