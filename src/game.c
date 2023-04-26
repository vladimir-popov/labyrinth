#include <stdio.h>
#include "game.h"

level
new_level (int rows, int cols, int seed)
{
  level level;
  level.lab = laby_generate (rows, cols, seed);
  level.player.row = 0;
  level.player.col = 0;

  return level;
}

int
handle_command (level *level, command cmd)
{
  return (cmd != CMD_EXIT);
}
