#include <stdio.h>
#include "game.h"

level
new_level (int rows, int cols, int seed)
{
  level level;
  level.lab = laby_generate (rows, cols, seed);
  level.player.r = 0;
  level.player.c = 0;

  return level;
}

int
handle_command (level *level, command cmd)
{
  return (cmd != CMD_EXIT);
}
