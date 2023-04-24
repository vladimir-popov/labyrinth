/*
 * This is an implementation of the Eller's algorithm.
 *
 * @see https://weblog.jamisbuck.org/2010/12/29/maze-generation-eller-s-algorithm
 * @see http://www.neocomputer.org/projects/eller.html
 */ 
#include <stdlib.h>

#include "laby.h"

#define get(R, C) laby_get_content (&lab, (R), (C))
#define set(R, C, V) laby_set_content (&lab, (R), (C), (V))

laby
eller_generate (int rows, int cols, int seed)
{
  srand (seed);

  /* The final labyrinth */
  laby lab = laby_init_empty (rows, cols);

  int set = 1;
  /* set unique set for every empty room in the first row */
  for (int j = 0; j < cols; j++)
    set (0, j, set++);

  for (int r = 0; r < rows - 1; r++)
    {
      /* decide if two rooms should have a horizontal border */
      for (int c = 0; c < cols - 1; c++)
        {
          if (get (r, c) != get (r, c + 1) && rand () % 2 == 0)
            laby_add_border (&lab, r, c, RIGHT_BORDER);
          else
            set (r, c + 1, get (r, c));
        }
      /* decide if two rooms should have a vertical border */
      for (int c = 0; c < cols; c++)
        {
          /* count of rooms without bottom border in the current set */
          int no_bb = 0;
          for (int i = 0; i < cols && no_bb < 2; i++)
            if (get (r, c) == get (r, i))
              no_bb = (laby_get_border (&lab, r, i) & BOTTOM_BORDER)
                          ? no_bb
                          : no_bb + 1;

          /* we can create a border, if it's not a single room without bottom
           * border in the set */
          if (no_bb > 1 && rand () % 5 > 0)
            {
              laby_add_border (&lab, r, c, BOTTOM_BORDER);
              /* mark the underlining room to change its set */
              set (r + 1, c, set++);
            }
          else
            set (r + 1, c, get (r, c));
        }
    }
  /* remove dead ends */
  for (int c = 0; c < cols - 1; c++)
    laby_rm_border (&lab, rows - 1, c, RIGHT_BORDER);

  return lab;
}
#undef get
#undef set

