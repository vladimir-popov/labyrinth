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
  KEY_EXIT
} key;

typedef struct player
{
  int row;
  int col;
} player;

typedef struct level
{
  laby lab;
  player player;
} level;

key read_key ();

void render (level *level);

level
new_level (int rows, int cols, int seed);

void
game_loop (level *level);

#endif /* __LABYRINTH_GAME__ */
