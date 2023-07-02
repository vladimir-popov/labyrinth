#include "game.h"
#include "render.h"
#include "term.h"
#include <stdlib.h>
#include <string.h>

/* This is semantic keys: */
enum key
{
  KEY_UNKNOWN,
  KEY_ENTER,
  KEY_CANCEL,
  KEY_LEFT,
  KEY_UP,
  KEY_RIGHT,
  KEY_DOWN,
  KEY_TOGGLE_MAP,
  KEY_CMD
};

/* This function transforms a pressed keyboard key to semantic key */
enum key
read_key ()
{
  KB_Code kp = read_pressed_key ();
  if (kp.len == 1)
    switch (kp.chars[0])
      {
      case KB_ESC:
        return KEY_CANCEL;
      case KB_ENTER:
        return KEY_ENTER;
      case 'j':
        return KEY_DOWN;
      case 'k':
        return KEY_UP;
      case 'h':
        return KEY_LEFT;
      case 'l':
        return KEY_RIGHT;
      case 'm':
        return KEY_TOGGLE_MAP;
      case ':':
        return KEY_CMD;
      }

  if (kp.len == 2)
    return KEY_UNKNOWN;

  if (kp.len == 3)
    switch (kp.chars[2])
      {
      case KB_DOWN:
        return KEY_DOWN;
      case KB_UP:
        return KEY_UP;
      case KB_LEFT:
        return KEY_LEFT;
      case KB_RIGHT:
        return KEY_RIGHT;
      }

  return KEY_UNKNOWN;
}

enum command
parse_cmd (char *cmd, int len)
{
  if (strcmp (cmd, "show all") == 0)
    return CMD_SHOW_ALL;

  if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "q") == 0)
    return CMD_EXIT;

  return CMD_CONTINUE;
}

/* This function transforms semantic key to the game command. */
enum command
read_command (Game *game)
{
  switch (GAME_STATE)
    {
    case ST_MAIN_MENU:
      {
        enum key key = read_key ();
        Menu *m = game->menu;
        switch (key)
          {
          case KEY_ENTER:
            return (m->option == M_NEW_GAME) ? CMD_NEW_GAME : CMD_EXIT;
          case KEY_CANCEL:
            return CMD_EXIT;
          case KEY_UP:
          case KEY_DOWN:
            m->option = (m->option == M_NEW_GAME) ? M_EXIT : M_NEW_GAME;
            return CMD_NOTHING;

          default:
            return CMD_NOTHING;
          }
      }

    case ST_GAME:
      {
        enum key key = read_key ();
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
          case KEY_TOGGLE_MAP:
            return CMD_SHOW_MAP;
          case KEY_CMD:
            return CMD_CMD;
          default:
            return CMD_NOTHING;
          }
      }
    case ST_MAP:
      {
        enum key key = read_key ();
        switch (key)
          {
          case KEY_TOGGLE_MAP:
            return CMD_CLOSE_MAP;
          case KEY_CANCEL:
            return CMD_PAUSE;
          case KEY_CMD:
            return CMD_CMD;
          default:
            return CMD_NOTHING;
          }
      }

    case ST_CMD:
      {
        KB_Code k = read_pressed_key ();
        if (k.len == 1)
          {
            switch (k.chars[0])
              {
              case KB_ESC:
                return CMD_CONTINUE;
              case KB_ENTER:
                return parse_cmd (M->cmd, M->option);
              case KB_BACKSPACE:
                if (M->option > 0)
                  M->option--;
                return CMD_NOTHING;
              default:
                if (M->option < MAX_CMD_LENGTH)
                  {
                    M->cmd[M->option] = k.chars[0];
                    M->option++;
                  }
                return CMD_NOTHING;
              }
          }
        else
          return CMD_NOTHING;
      }

    case ST_PAUSE:
      {
        enum key key = read_key ();
        Menu *m = game->menu;
        switch (key)
          {
          case KEY_UP:
          case KEY_DOWN:
            m->option = (m->option == M_CONTINUE) ? M_EXIT : M_CONTINUE;
            break;

          case KEY_CANCEL:
            return CMD_CONTINUE;

          case KEY_ENTER:
            if (m->option == M_CONTINUE)
              return CMD_CONTINUE;
            else
              return CMD_EXIT;

          default:
            return CMD_NOTHING;
          }
      }

    case ST_WIN:
      {
        enum key key = read_key ();
        switch (key)
          {
          case KEY_CANCEL:
          case KEY_ENTER:
            return CMD_NEW_GAME;
          default:
            return CMD_NOTHING;
          }
      }
    }
}
