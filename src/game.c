/**
 * This file contains the main logic of the game,
 * which is not depend on a runtime.
 */
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

static const int CONTINUE_LOOP = 1;

static const int STOP_LOOP = 0;

static void
generate_new_level (level *level, int height, int width, int seed)
{
  laby_generate (&level->lab, height, width, seed);
  level->player.y = 0;
  level->player.x = 0;
  level->player.visible_range = 2;
  level->exit.y = 1; // height - 1;
  level->exit.x = 1; // width - 1;
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

static void
move_player (game *game, int dy, int dx)
{
  game->level.player.y += dy;
  game->level.player.x += dx;
  for (int i = 0; i < game->level.visible_rooms_count; i++)
    ROOM_MARK_AS_NOT_VISIBLE (game->level.visible_rooms[i]);

  game->level.visible_rooms_count = 0;
  free (game->level.visible_rooms);
  game->level.visible_rooms = NULL;
}

static int
handle_cmd_in_game (game *game, enum command cmd)
{
  level *level = &game->level;
  player *p = &level->player;
  char border = laby_get_border (&level->lab, p->y, p->x);
  switch (cmd)
    {
    case CMD_MV_LEFT:
      if ((p->x > 0) && !(border & LEFT_BORDER))
        move_player (game, 0, -1);
      break;
    case CMD_MV_UP:
      if ((p->y > 0) && !(border & UPPER_BORDER))
        move_player (game, -1, 0);
      break;
    case CMD_MV_RIGHT:
      if ((p->x < level->lab.width - 1) && !(border & RIGHT_BORDER))
        move_player (game, 0, 1);
      break;
    case CMD_MV_DOWN:
      if ((p->y < level->lab.height - 1) && !(border & BOTTOM_BORDER))
        move_player (game, 1, 0);
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
mark_visible_rooms (level *level)
{
  player *p = &level->player;

  if (level->visible_rooms == NULL)
    level->visible_rooms_count = laby_find_visible_rooms (
        &level->lab, &level->visible_rooms, p->y, p->x, p->visible_range);

  for (int i = 0; i < level->visible_rooms_count; i++)
    ROOM_MARK_AS_VISIBLE (level->visible_rooms[i]);
}

void
game_loop (game *game)
{
  enum command cmd;
  do
    {
      if (game->state == ST_GAME)
        mark_visible_rooms (&game->level);
      render (game);
      cmd = read_command (game);
    }
  while (handle_command (game, cmd));
}
