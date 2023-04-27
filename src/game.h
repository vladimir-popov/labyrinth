#ifndef __LABYRINTH_GAME__
#define __LABYRINTH_GAME__

#include "laby.h"

typedef enum
{
  CMD_MV_LEFT = 1000,
  CMD_MV_UP,
  CMD_MV_RIGHT,
  CMD_MV_DOWN,
  CMD_EXIT
} command;

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
    LABY_EMPTY, PLAYER_EMPTY                                                 \
  }

level new_level (int rows, int cols, int seed);

int handle_command (level *level, command cmd);

#endif /* __LABYRINTH_GAME__ */
