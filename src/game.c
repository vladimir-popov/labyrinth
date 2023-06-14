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
  laby_generate (&L, game->height, game->width, game->seed);
  game->player.row = 0;
  game->player.col = 0;
  game->player.visible_range = 2;
  laby_set_content (&L, P.row, P.col, C_PLAYER);
  laby_set_content (&L, game->height - 1, game->width - 1, C_EXIT);
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
  /* unmark visible rooms */
  for (int i = P.row - P.visible_range; i <= P.row + P.visible_range; i++)
    for (int j = P.col - P.visible_range; j <= P.col + P.visible_range; j++)
      {
        laby_set_visibility (&L, i, j, 0);
      }
  /* change player's position */
  laby_set_content (&L, P.row, P.col, C_NOTHING);
  P.row += dr;
  P.col += dc;
  if (laby_get_content (&L, P.row, P.col) == C_EXIT)
    {
      game->state = ST_WIN;
      game->menu = create_menu (game, ST_WIN);
    }
  else
    {
      laby_set_content (&L, P.row, P.col, C_PLAYER);
      /* mark new visible rooms */
      laby_mark_visible_rooms (&L, P.row, P.col, P.visible_range);
    }
}

static int
handle_cmd_in_game (Game *game, enum command cmd)
{
  char border = laby_get_borders (&L, P.row, P.col);
  switch (cmd)
    {
    case CMD_MV_LEFT:
      if ((P.col > 0) && !(border & LEFT_BORDER))
        move_player (game, 0, -1);
      break;
    case CMD_MV_UP:
      if ((P.row > 0) && !(border & UPPER_BORDER))
        move_player (game, -1, 0);
      break;
    case CMD_MV_RIGHT:
      if ((P.col < L.cols - 1) && !(border & RIGHT_BORDER))
        move_player (game, 0, 1);
      break;
    case CMD_MV_DOWN:
      if ((P.row < L.rows - 1) && !(border & BOTTOM_BORDER))
        move_player (game, 1, 0);
      break;
    case CMD_PAUSE:
      game->state = ST_PAUSE;
      game->menu = create_menu (game, ST_PAUSE);
      break;
    default:
      break;
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
