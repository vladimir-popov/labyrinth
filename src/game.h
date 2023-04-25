/**
 * This is the main "engine" of the game.
 * It uses some external functions to be independent on graphical environment,
 * and implementation details.
 */
#ifndef __LABYRINTH_GAME__
#define __LABYRINTH_GAME__

#include "laby.h"

typedef enum key
{
  KEY_LEFT,
  KEY_UP,
  KEY_RIGHT,
  KEY_DOWN,
  KEY_COMMAND,
  KEY_ESC
} key;

typedef struct player
{
  int row;
  int col;
} player;

typedef struct game
{
  laby level;
  player p;
} game;

key read_key ();

void render (game *game);

game
new_game (int rows, int cols, int seed);

void
game_loop (game *game);

#endif /* __LABYRINTH_GAME__ */
