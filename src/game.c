/**
 * This file contains the main logic of the game,
 * which is not depend on a runtime.
 */
#include "game.h"
#include <stdio.h>

static const int CONTINUE_LOOP = 1;

static const int STOP_LOOP = 0;

static void
generate_new_level (level *level, int height, int width, int seed)
{
  laby_generate (&level->lab, height, width, seed);
  level->player.y = 0;
  level->player.x = 0;
  level->exit.y = 1;// height - 1;
  level->exit.x = 1;// width - 1;
}

static int
handle_cmd_in_main_menu (game *game, enum command cmd)
{
  switch (cmd)
    {
    case CMD_NEW_GAME:
      generate_new_level (&game->level, game->height, game->width, game->seed);
      game->state = ST_GAME;
      close_menu (game->menu, ST_MAIN_MENU);
      game->menu = NULL;
      return CONTINUE_LOOP;

    case CMD_EXIT:
      return STOP_LOOP;

    default:
      return CONTINUE_LOOP;
    }
}

static int
handle_cmd_in_game (game *game, enum command cmd)
{
  level *level = &game->level;
  position *p = &level->player;
  char border = laby_get_border (&game->level.lab, p->y, p->x);
  switch (cmd)
    {
    case CMD_MV_LEFT:
      if ((p->x > 0) && !(border & LEFT_BORDER))
        p->x--;
      break;
    case CMD_MV_UP:
      if ((p->y > 0) && !(border & UPPER_BORDER))
        p->y--;
      break;
    case CMD_MV_RIGHT:
      if ((p->x < level->lab.width - 1) && !(border & RIGHT_BORDER))
        p->x++;
      break;
    case CMD_MV_DOWN:
      if ((p->y < level->lab.height - 1) && !(border & BOTTOM_BORDER))
        p->y++;
      break;
    case CMD_PAUSE:
      game->state = ST_PAUSE;
      game->menu = create_menu (game, ST_PAUSE);
      break;
    default:
      break;
    }
  if (p->y == level->exit.y && p->x == level->exit.x)
    {
      game->state = ST_WIN;
      game->menu = create_menu (game, ST_WIN);
    }
  return CONTINUE_LOOP;
}

static int
handle_cmd_in_pause (game *game, enum command cmd)
{
  switch (cmd)
    {
    case CMD_CONTINUE:
      game->state = ST_GAME;
      close_menu (game->menu, ST_GAME);
      game->menu = NULL;
      return CONTINUE_LOOP;

    case CMD_EXIT:
      return STOP_LOOP;

    default:
      return CONTINUE_LOOP;
    }

  return CONTINUE_LOOP;
}

static int
handle_cmd_in_win (game *game, enum command cmd)
{
  switch (cmd)
    {
    case CMD_EXIT:
      return STOP_LOOP;
    default:
      return CONTINUE_LOOP;
    }
}

int
handle_command (game *game, enum command cmd)
{
  switch (game->state)
    {
    case ST_MAIN_MENU:
      return handle_cmd_in_main_menu (game, cmd);
    case ST_GAME:
      return handle_cmd_in_game (game, cmd);
    case ST_PAUSE:
      return handle_cmd_in_pause (game, cmd);
    case ST_WIN:
      return handle_cmd_in_win (game, cmd);
    }
}

void
game_init (game *game, int height, int width, int seed)
{
  game->seed = seed;
  game->height = height;
  game->width = width;
  game->state = ST_MAIN_MENU;
  game->menu = create_menu (NULL, ST_MAIN_MENU);
}

void
game_loop (game *game)
{
  enum command cmd;
  do
    {
      render (game);
      cmd = read_command (game);
    }
  while (handle_command (game, cmd));
}
