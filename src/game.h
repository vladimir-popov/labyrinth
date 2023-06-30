#ifndef __LABYRINTH_GAME__
#define __LABYRINTH_GAME__

#include "laby.h"

#define L game->lab
#define P game->player
#define M game->menu

enum command
{
  /* This command means that nothing happened,
   * and run loop should be continued */
  CMD_NOTHING = 0,
  /* Run a new game */
  CMD_NEW_GAME = 1000,
  /* Move the player on one room left */
  CMD_MV_LEFT,
  /* Move the player on one room up */
  CMD_MV_UP,
  /* Move the player on one room right */
  CMD_MV_RIGHT,
  /* Move the player on one room down */
  CMD_MV_DOWN,
  /* Put the game on pause and show menu */
  CMD_PAUSE,
  /* Close pause menu and continue the game */
  CMD_CONTINUE,
  /* Exit from the game */
  CMD_EXIT,
  /* Command to wait a special command from user */
  CMD_CMD,
  /* Cheat to show whole labyrinth */
  CMD_SHOW_ALL
};

enum game_state
{
  /* The welcome screen is shown */
  ST_MAIN_MENU,
  /* The game in a process */
  ST_GAME,
  /* The game is on pause and pause menu is shown */
  ST_PAUSE,
  /* The message about victory is shown */
  ST_WIN,
  /* The game is on pause and waiting a special command from user */
  ST_CMD
};

struct render;
typedef struct render Render;

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
  /* Global configuration of the game */
  lcg seed;
  /* The count of rooms by vertical in the new laby */
  int laby_rows;
  /* The count of rooms by horizontal in the new laby */
  int laby_cols;

  Render *render;
  /* ------------------------------- */

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

void game_run_loop (Game *game, Render *render);

/* ========== THIS FUNCTION RUNTIME DEPENDED ========== */

enum command read_command (Game *game);

int handle_command (Game *game, enum command cmd);

void render (Render *render, Game *game);

Menu *create_menu (enum game_state state);

void close_menu (Menu *menu, enum game_state state);

#endif /* __LABYRINTH_GAME__ */
