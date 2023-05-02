#ifndef __LABYRINTH_GAME__
#define __LABYRINTH_GAME__

#include <time.h>
#include "laby.h"

enum command
{
  CMD_NOTHING = 0,
  CMD_NEW_GAME = 1000,
  CMD_MV_LEFT,
  CMD_MV_UP,
  CMD_MV_RIGHT,
  CMD_MV_DOWN,
  CMD_EXIT,
};

enum game_state
{
  ST_MAIN_MENU,
  ST_GAME,
  ST_PAUSE
};

typedef struct
{
  int r;
  int c;
} player;

#define PLAYER_EMPTY                                                          \
  {                                                                           \
    0, 0                                                                      \
  }

typedef struct
{
  laby lab;
  player player;
} level;

#define LEVEL_EMPTY                                                           \
  {                                                                           \
    LABY_EMPTY, PLAYER_EMPTY                                                  \
  }

typedef struct
{
  int seed;
  int rows;
  int cols;
  enum game_state state;
  level level;
  /* Implementation of a menu depends on runtime.
   * The main logic of the game doesn't depend on a menu
   * implementation.*/
  void *menu;
} game;

game game_init (int rows, int cols, int seed);

void game_loop (game *game);

/* ========== THIS FUNCTION RUNTIME DEPENDED ========== */

enum command read_command (game *game);

int handle_command (game *game, enum command cmd);

void* create_menu (const game *game, enum game_state state);

void close_menu (void *menu, enum game_state state);

void render (game *game, double time_frame_s);

#endif /* __LABYRINTH_GAME__ */
