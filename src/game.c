/**
 * This file contains the main logic of the game,
 * which is not depend on a runtime.
 */
#include "game.h"
#include <stdio.h>

static level
generate_new_level (int rows, int cols, int seed)
{
  level level;
  level.lab = laby_generate (rows, cols, seed);
  level.player.r = 0;
  level.player.c = 0;

  return level;
}

static int
handle_cmd_in_main_menu (game *game, enum command cmd)
{
  switch (cmd)
    {
    case CMD_NEW_GAME:
      game->level = generate_new_level (game->rows, game->cols, game->seed);
      game->state = ST_GAME;
      close_menu (game->menu, ST_MAIN_MENU);
      game->menu = NULL;
      return 1;
    case CMD_EXIT:
      return 0;
    default:
      return 1;
    }
}

static int
handle_cmd_in_game (game *game, enum command cmd)
{
  level *level = &game->level;
  player *p = &level->player;
  char border = laby_get_border (&game->level.lab, p->r, p->c);
  switch (cmd)
    {
    case CMD_MV_LEFT:
      if ((p->c > 0) && !(border & LEFT_BORDER))
        p->c--;
      break;
    case CMD_MV_UP:
      if ((p->r > 0) && !(border & UPPER_BORDER))
        p->r--;
      break;
    case CMD_MV_RIGHT:
      if ((p->c < level->lab.cols_count - 1) && !(border & RIGHT_BORDER))
        p->c++;
      break;
    case CMD_MV_DOWN:
      if ((p->r < level->lab.rows_count - 1) && !(border & BOTTOM_BORDER))
        p->r++;
      break;
    case CMD_EXIT:
      return 0;
    default:
      return 1;
    }
  return 1;
}

static int
handle_cmd_in_pause (game *game, enum command cmd)
{
  return 1;
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
    }
}

game
game_init (int rows, int cols, int seed)
{
  void *menu = create_menu (NULL, ST_MAIN_MENU);
  game g = { seed, rows, cols, ST_MAIN_MENU, LEVEL_EMPTY, menu };
  return g;
}

void
game_loop (game *game)
{
  enum command cmd;
  time_t prev_render_at = time (NULL);
  do
    {
      time_t now = time (NULL);
      render (game, difftime (now, prev_render_at));
      prev_render_at = now;

      cmd = read_command (game);
    }
  while (handle_command (game, cmd));
}
