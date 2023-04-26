/**
 * This is the main "engine" of the game.
 * It uses some external functions to be independent on graphical environment,
 * and implementation details.
 */
#include "game.h"
#include <stdio.h>

level
new_level (int rows, int cols, int seed)
{
  level level;
  level.lab = laby_generate (rows, cols, seed);
  level.player.row = 0;
  level.player.col = 0;

  return level;
}

static int
handle_key (level *level, key key)
{
  return (key != KEY_EXIT);
}

void
game_loop (level *level)
{
  key k;
  do
    {
      render (level);
      k = read_key ();
    }
  while (handle_key (level, k));
}
