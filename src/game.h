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
  unsigned char y;
  unsigned char x;
  /* The radius of visible rooms circle including the y:x.
   * 1 means that only current y:x room is visible. 0 turns off visibility
   * check completely. */
  unsigned char visible_range;
} player;

#define PLAYER_DEFAULT                                                        \
  {                                                                           \
    0, 0, 0                                                                   \
  }

typedef struct
{
  laby lab;
  player player;
  pos exit;
  p_room *visible_rooms;
  unsigned char visible_rooms_count;
} level;

#define LEVEL_EMPTY                                                           \
  {                                                                           \
    LABY_EMPTY, PLAYER_DEFAULT, POSITION_EMPTY, NULL, 0                       \
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

#define GAME_EMPTY                                                            \
  {                                                                           \
    0, 0, 0, ST_MAIN_MENU, LEVEL_EMPTY, NULL                                  \
  }

/**
 * Initializes a game with count of rooms in the labyrinth and seed number.
 */
void game_init (game *game, int height, int width, int seed);

/* Finds all visible rooms and marks them. */
// FIXME: it's opened for test, but it looks weird
void mark_visible_rooms (level *level);

void game_loop (game *game);

/* ========== THIS FUNCTION RUNTIME DEPENDED ========== */

enum command read_command (game *game);

int handle_command (game *game, enum command cmd);

void *create_menu (const game *game, enum game_state state);

void close_menu (void *menu, enum game_state state);

void render (game *game);

#endif /* __LABYRINTH_GAME__ */
