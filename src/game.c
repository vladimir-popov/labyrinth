#include "game.h"
#include "eller.c"
#include "laby.h"

game
new_game (int rows, int cols, int seed)
{
  game game;
  game.level = eller_generate (rows, cols, seed);

  return game;
}

void
game_loop (game *game)
{
  render (game);
}
