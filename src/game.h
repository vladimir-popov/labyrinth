#ifndef __LABYRINTH_GAME__
#define __LABYRINTH_GAME__

#include "laby.h"
#include <time.h>

enum command
{
  CMD_NOTHING = 0,
  CMD_NEW_GAME = 1000,
  CMD_MV_LEFT,
  CMD_MV_UP,
  CMD_MV_RIGHT,
  CMD_MV_DOWN,
  CMD_PAUSE,
  CMD_CONTINUE,
  CMD_EXIT,
};

enum game_state
{
  ST_MAIN_MENU,
  ST_GAME,
  ST_PAUSE,
  ST_WIN
};

typedef struct
{
  int y;
  int x;
} position;

#define POSITION_EMPTY                                                        \
  {                                                                           \
    0, 0                                                                      \
  }

typedef struct
{
  laby lab;
  position player;
  position exit;
} level;

#define LEVEL_EMPTY                                                           \
  {                                                                           \
    LABY_EMPTY, POSITION_EMPTY, POSITION_EMPTY                                \
  }

typedef struct
{
  int seed;
  /* The count of rooms by vertical */
  int height;
  /* The count of rooms by horizontal */
  int width;
  /* The FSM of the game  */
  enum game_state state;
  /* The current level */
  level level;
  /* Implementation of a menu depends on runtime.
   * The main logic of the game doesn't depend on a menu
   * implementation.*/
  void *menu;
} game;

/**
 * Initializes a game with count of rooms in the labyrinth and seed number.
 */
void game_init (game *game, int height, int width, int seed);

void game_loop (game *game);

/* ========== THIS FUNCTION RUNTIME DEPENDED ========== */

enum command read_command (game *game);

int handle_command (game *game, enum command cmd);

void *create_menu (const game *game, enum game_state state);

void close_menu (void *menu, enum game_state state);

void render (game *game);

#endif /* __LABYRINTH_GAME__ */
