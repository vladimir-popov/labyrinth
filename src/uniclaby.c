#include "dstr.c"
#include "laby.c"
#include <stdio.h>

/* Renders a labyrinth `lab` to the buffer `buf`. */
void
uc_render_laby_compact (dstr *buf, laby *lab)
{
  char *room = "═╬═";
  for (int r = 0; r < lab->rows_count; r++)
    {
      for (int c = 0; c < lab->cols_count; c++)
        {
          switch (laby_get_border (lab, r, c))
            {
            case (LEFT_BORDER | UPPER_BORDER | RIGHT_BORDER | BOTTOM_BORDER):
              room = "   ";
              break;
            case LEFT_BORDER:
              room = " ╠═";
              break;
            case RIGHT_BORDER:
              room = "═╣ ";
              break;
            case UPPER_BORDER:
              room = "═╦═";
              break;
            case BOTTOM_BORDER:
              room = "═╩═";
              break;
            case (UPPER_BORDER | BOTTOM_BORDER):
              room = "═══";
              break;
            case (LEFT_BORDER | RIGHT_BORDER):
              room = " ║ ";
              break;
            case (LEFT_BORDER | UPPER_BORDER):
              room = " ╔═";
              break;
            case (RIGHT_BORDER | UPPER_BORDER):
              room = "═╗ ";
              break;
            case (LEFT_BORDER | BOTTOM_BORDER):
              room = " ╚═";
              break;
            case (RIGHT_BORDER | BOTTOM_BORDER):
              room = "═╝ ";
              break;
            case (LEFT_BORDER | UPPER_BORDER | RIGHT_BORDER):
              room = " ╓ ";
              break;
            case (UPPER_BORDER | RIGHT_BORDER | BOTTOM_BORDER):
              room = "╕  ";
              break;
            case (RIGHT_BORDER | BOTTOM_BORDER | LEFT_BORDER):
              room = " ╙ ";
              break;
            case (BOTTOM_BORDER | LEFT_BORDER | UPPER_BORDER):
              room = "  ╘";
              break;
            default:
              room = "═╬═";
              break;
            }
          dstr_append (buf, room, strlen (room));
        }
      if (r < lab->rows_count - 1)
        dstr_append (buf, "\r\n", 2);
    }
}
