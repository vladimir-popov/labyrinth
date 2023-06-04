/**
 * This file contains the main logic of the game,
 * which is not depend on a runtime.
 */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

static const int CONTINUE_LOOP = 1;

static const int STOP_LOOP = 0;

void
game_init (Game *game, int height, int width, int seed)
{
  game->seed = seed;
  game->height = height;
  game->width = width;
  game->state = ST_MAIN_MENU;
  game->menu = create_menu (NULL, ST_MAIN_MENU);
}

void
game_run_loop (Game *game)
{
  enum command cmd;
  do
    {
      render (game);
      cmd = read_command (game);
    }
  while (handle_command (game, cmd));
}

static void
generate_new_level (Game *game)
{
  laby_generate (&game->lab, game->height, game->width, game->seed);
  game->player.row = 0;
  game->player.col = 0;
  game->player.visible_range = 3;
  game->exit.row = game->height - 1;
  game->exit.col = game->width - 1;
}

static int
handle_cmd_in_main_menu (Game *game, enum command cmd)
{
  switch (cmd)
    {
    case CMD_NEW_GAME:
      generate_new_level (game);
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

static void
move_player (Game *game, int dr, int dc)
{
  game->player.row += dr;
  game->player.col += dc;
}

static int
handle_cmd_in_game (Game *game, enum command cmd)
{
  Player *p = &game->player;
  char border = laby_get_border (&game->lab, p->row, p->col);
  switch (cmd)
    {
    case CMD_MV_LEFT:
      if ((p->col > 0) && !(border & LEFT_BORDER))
        move_player (game, 0, -1);
      break;
    case CMD_MV_UP:
      if ((p->row > 0) && !(border & UPPER_BORDER))
        move_player (game, -1, 0);
      break;
    case CMD_MV_RIGHT:
      if ((p->col < game->lab.cols - 1) && !(border & RIGHT_BORDER))
        move_player (game, 0, 1);
      break;
    case CMD_MV_DOWN:
      if ((p->row < game->lab.rows - 1) && !(border & BOTTOM_BORDER))
        move_player (game, 1, 0);
      break;
    case CMD_PAUSE:
      game->state = ST_PAUSE;
      game->menu = create_menu (game, ST_PAUSE);
      break;
    default:
      break;
    }
  if (p->row == game->exit.row && p->col == game->exit.col)
    {
      game->state = ST_WIN;
      game->menu = create_menu (game, ST_WIN);
    }
  return CONTINUE_LOOP;
}

static int
handle_cmd_in_pause (Game *game, enum command cmd)
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
handle_cmd_in_win (Game *game, enum command cmd)
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
handle_command (Game *game, enum command cmd)
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
