#ifndef __LABYRINTH_GAME__
#define __LABYRINTH_GAME__

#include "laby.h"


#define L game->lab
#define P game->player
#define E game->exit

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

struct menu;
typedef struct menu Menu;

typedef struct
{
  unsigned char row;
  unsigned char col;
  /* The radius of visible distance including the y:x.
   * 1 means that only current y:x room is visible.
   * 0 turns off visibility check completely. */
  unsigned char visible_range;
} Player;

typedef struct
{
  int seed;
  /* The count of rooms by vertical */
  int laby_rows;
  /* The count of rooms by horizontal */
  int laby_cols;
  /* The FSM of the game  */
  enum game_state state;
  /* The current labyrinth */
  Laby lab;
  /* The current state of the player */
  Player player;
  /* Implementation of a menu depends on runtime.
   * The main logic of the game doesn't depend on a menu
   * implementation.*/
  Menu *menu;
} Game;

/**
 * Initializes a game with count of rooms in the labyrinth and seed number.
 */
void game_init (Game *game, int rows, int cols, int seed);

void game_run_loop (Game *game);

/* ========== THIS FUNCTION RUNTIME DEPENDED ========== */

enum command read_command (Game *game);

int handle_command (Game *game, enum command cmd);

void *create_menu (const Game *game, enum game_state state);

void close_menu (void *menu, enum game_state state);

void render (Game *game);

#endif /* __LABYRINTH_GAME__ */
