/**
 * This file contains the main logic of the game,
 * which is not depend on a runtime.
 */
#include "game.h"
#include "lcg.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const int CONTINUE_LOOP = 1;

static const int STOP_LOOP = 0;

static void
game_set_state (Game *game, enum game_state state)
{
  game->state_idx++;
  assert (game->state_idx < MAX_STATES_STACK_SIZE);
  game->states_stack[game->state_idx] = state;
}

static void
game_recover_prev_state (Game *game)
{
  assert (game->state_idx > 0);
  game->state_idx--;
}

void
game_init (Game *game, int height, int width, int seed)
{
  game->seed = seed;
  game->laby_rows = height;
  game->laby_cols = width;
  game->state_idx = 0;
  game->states_stack
      = malloc (sizeof (enum game_state) * MAX_STATES_STACK_SIZE);
  game->states_stack[0] = ST_MAIN_MENU;
  game->menu = create_menu (ST_MAIN_MENU);
}

void
game_run_loop (Game *game, Render *r)
{
  enum command cmd;
  do
    {
      render (r, game);
      cmd = read_command (game);
    }
  while (handle_command (game, cmd));
}

static void
game_init_player (Game *game)
{
  game->player.row = lcg_rand (&game->seed) % L.rows;
  game->player.col = lcg_rand (&game->seed) % L.cols;
  game->player.visible_range = 2;
  laby_set_content (&L, P.row, P.col, C_PLAYER);
  laby_mark_visible_rooms (&L, P.row, P.col, P.visible_range);
}

static void
game_place_exit (Game *game)
{
  double a = (lcg_rand (&game->seed) % 360) * M_PI / 180;
  int r = 2 * P.visible_range * sin (a) + P.row;
  int c = 2 * P.visible_range * cos (a) + P.col;
  r = (r < 0) ? 0 : (r >= L.rows) ? L.rows - 1 : r;
  c = (c < 0) ? 0 : (c >= L.cols) ? L.cols - 1 : c;
  if (r == P.row && c == P.col)
    game_place_exit (game);
  else
    laby_set_content (&L, r, c, C_EXIT);
}

static void
generate_new_level (Game *game)
{
  laby_generate (&L, game->laby_rows, game->laby_cols, &game->seed);
  game_init_player (game);
  game_place_exit (game);
}

static int
handle_cmd_in_main_menu (Game *game, enum command cmd)
{
  switch (cmd)
    {
    case CMD_NEW_GAME:
      generate_new_level (game);
      game_set_state (game, ST_GAME);
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
  /* check collisions */
  if (laby_get_content (&L, P.row, P.col) == C_EXIT)
    {
      game_set_state (game, ST_WIN);
      game->menu = create_menu (ST_WIN);
    }
  else
    {
      laby_set_content (&L, P.row, P.col, C_PLAYER);
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
      game_set_state (game, ST_PAUSE);
      game->menu = create_menu (ST_PAUSE);
      break;
    case CMD_SHOW_MAP:
      game_set_state (game, ST_MAP);
      break;
    case CMD_CMD:
      game_set_state (game, ST_CMD);
      game->menu = create_menu (ST_CMD);
      break;
    default:
      break;
    }
  return CONTINUE_LOOP;
}

static int
handle_cmd_in_map (Game *game, enum command cmd)
{
  switch (cmd)
    {
    case CMD_PAUSE:
      game_set_state (game, ST_PAUSE);
      game->menu = create_menu (ST_PAUSE);
      break;
    case CMD_CLOSE_MAP:
      game_recover_prev_state (game);
      break;
    case CMD_CMD:
      game_set_state (game, ST_CMD);
      game->menu = create_menu (ST_CMD);
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
      game_recover_prev_state (game);
      close_menu (game->menu, ST_PAUSE);
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
    case CMD_NEW_GAME:
      close_menu (game->menu, ST_WIN);
      game_set_state (game, ST_MAIN_MENU);
      game->menu = create_menu (ST_MAIN_MENU);
      return CONTINUE_LOOP;
    default:
      return CONTINUE_LOOP;
    }
}

static int
handle_cmd_in_cmd_mode (Game *game, enum command cmd)
{
  switch (cmd)
    {
    case CMD_CONTINUE:
      game_recover_prev_state (game);
      close_menu (game->menu, ST_CMD);
      game->menu = NULL;
      return CONTINUE_LOOP;
    case CMD_SHOW_ALL:
      game_recover_prev_state (game);
      close_menu (game->menu, ST_CMD);
      game->menu = NULL;
      laby_mark_whole_as_known (&L);
      return CONTINUE_LOOP;
    default:
      return CONTINUE_LOOP;
    }
}

int
handle_command (Game *game, enum command cmd)
{
  switch (GAME_STATE)
    {
    case ST_MAIN_MENU:
      return handle_cmd_in_main_menu (game, cmd);
    case ST_GAME:
      return handle_cmd_in_game (game, cmd);
    case ST_MAP:
      return handle_cmd_in_map (game, cmd);
    case ST_PAUSE:
      return handle_cmd_in_pause (game, cmd);
    case ST_WIN:
      return handle_cmd_in_win (game, cmd);
    case ST_CMD:
      return handle_cmd_in_cmd_mode (game, cmd);
    }
}
