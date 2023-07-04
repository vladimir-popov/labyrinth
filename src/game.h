#ifndef __LABYRINTH_GAME__
#define __LABYRINTH_GAME__

#include "laby.h"

/* The max count of states in the stack of game states
 * is limited by logic and should not be overflowed  */
#define MAX_STATES_STACK_SIZE 5

/* The macros to take the current game state */
#define GAME_STATE game->states_stack[game->state_idx]
#define GAME_PREV_STATE                                                       \
  ((game->state_idx > 0) ? game->states_stack[game->state_idx - 1] : 0)

/* A few macros to simplify work with game objects */
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
  /* Close the current menu or map and continue the game */
  CMD_CONTINUE,
  /* Exit from the game */
  CMD_EXIT,
  /* Command to wait a special command from user */
  CMD_CMD,
  /* Command to show the map */
  CMD_SHOW_MAP,
  /* Show keys settings menu */
  CMD_SHOW_KEYS_SETTINGS,
  /* Cheat to show whole labyrinth */
  CMD_SHOW_ALL
};

enum game_state
{
  /* The welcome screen is shown */
  ST_WELCOME_SCREEN,
  /* The game in a process */
  ST_GAME,
  /* The map is shown */
  ST_MAP,
  /* The game is on pause and pause menu is shown */
  ST_PAUSE,
  /* The keys settings menu is shown */
  ST_KEY_SETTINGS,
  /* The message about victory is shown */
  ST_WIN,
  /* The game is on pause and waiting a special command from user */
  ST_CMD
};

enum laby_draw_mode
{
  /* Only visible part of the laby should be drawn */
  DLM_REGULAR,
  /* Only know part of the laby should be drawn (the "map" mode) */
  DLM_MAP,
  /* The whole laby should be drawn (in case of cheat as example) */
  DLM_WHOLE
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
  /* ------------------------------- */
  lcg seed;
  /* The count of rooms by vertical in the new laby */
  int laby_rows;
  /* The count of rooms by horizontal in the new laby */
  int laby_cols;

  Render *render;
  /* ------------------------------- */

  /* The FSM of the game with history */
  enum game_state *states_stack;
  /* The index of the current game state */
  unsigned char state_idx;
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

void menu_next_option (Menu *menu);

void menu_prev_option (Menu *menu);

/* ========== THIS FUNCTION RUNTIME DEPENDED ========== */

enum command read_command (Game *game);

int handle_command (Game *game, enum command cmd);

void render (Render *render, Game *game);

Menu *create_menu (enum game_state state);

void close_menu (Menu *menu, enum game_state state);

#endif /* __LABYRINTH_GAME__ */
