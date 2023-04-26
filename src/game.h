#ifndef __LABYRINTH_GAME__
#define __LABYRINTH_GAME__

#include "laby.h"

typedef enum
{
  CMD_MV_LEFT,
  CMD_MV_UP,
  CMD_MV_RIGHT,
  CMD_MV_DOWN,
  CMD_EXIT
} command;

typedef struct
{
  int row;
  int col;
} player;

typedef struct
{
  laby lab;
  player player;
} level;

level new_level (int rows, int cols, int seed);

int handle_command (level *level, command cmd);

#endif /* __LABYRINTH_GAME__ */
