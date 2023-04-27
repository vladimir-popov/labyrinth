#include "game.h"
#include <stdio.h>

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
  player *p = &level->player;
  char border = laby_get_border (&level->lab, p->r, p->c);
  switch (cmd)
    {
    case CMD_MV_LEFT:
      if ((p->c > 0) && !(border & LEFT_BORDER))
        p->c--;
      break;
    case CMD_MV_UP:
      if ((p->r > 0) && !(border & UPPER_BORDER))
        p->r--;
      break;
    case CMD_MV_RIGHT:
      if ((p->c < level->lab.cols_count - 1) && !(border & RIGHT_BORDER))
        p->c++;
      break;
    case CMD_MV_DOWN:
      if ((p->r < level->lab.rows_count - 1) && !(border & BOTTOM_BORDER))
        p->r++;
      break;
    case CMD_EXIT:
      return 0;
    }
  return 1;
}
