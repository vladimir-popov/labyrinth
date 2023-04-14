#include "dstr.c"
#include "laby.c"
#include <stdio.h>

/* A room without borders */
#define UC_EMPTY "═╬═"
/* A closed room */
#define UC_CLOSED "   "
/* A room with left border */
#define UC_LB " ╠═"
/* A room with right border */
#define UC_RB "═╣ "
/* A room with upper border */
#define UC_UB "═╦═"
/* A room with bottom border */
#define UC_BB "═╩═"
/* A room with upper and bottom borders */
#define UC_UBB "═══"
/* A room with left and right borders */
#define UC_LRB " ║ "
/* A room with left and upper borders */
#define UC_LUB " ╔═"
/* A room with right and upper borders */
#define UC_RUB "═╗ "
/* A room with left and bottom borders */
#define UC_LBB " ╚═"
/* A room with right and bottom borders */
#define UC_RBB "═╝ "
/* A room with bottom entrance */
#define UC_LURB " ╓ "
/* A room with left entrance */
#define UC_URBB "╕  "
/* A room with upper entrance */
#define UC_RBLB " ╙ "
/* A room with right entrance */
#define UC_BLUB "  ╘"

/* Renders a labyrinth `lab` to the buffer `buf`. */
void
uc_render_laby (dstr *buf, laby *lab)
{
  char *room = UC_EMPTY;
  for (int r = 0; r < lab->rows_count; r++)
    {
      for (int c = 0; c < lab->cols_count; c++)
        {
          switch (laby_get_border (lab, r, c))
            {
            case (LEFT_BORDER | UPPER_BORDER | RIGHT_BORDER | BOTTOM_BORDER):
              room = UC_CLOSED;
              break;
            case LEFT_BORDER:
              room = UC_LB;
              break;
            case RIGHT_BORDER:
              room = UC_RB;
              break;
            case UPPER_BORDER:
              room = UC_UB;
              break;
            case BOTTOM_BORDER:
              room = UC_BB;
              break;
            case (UPPER_BORDER | BOTTOM_BORDER):
              room = UC_UBB;
              break;
            case (LEFT_BORDER | RIGHT_BORDER):
              room = UC_LRB;
              break;
            case (LEFT_BORDER | UPPER_BORDER):
              room = UC_LUB;
              break;
            case (RIGHT_BORDER | UPPER_BORDER):
              room = UC_RUB;
              break;
            case (LEFT_BORDER | BOTTOM_BORDER):
              room = UC_LBB;
              break;
            case (RIGHT_BORDER | BOTTOM_BORDER):
              room = UC_RBB;
              break;
            case (LEFT_BORDER | UPPER_BORDER | RIGHT_BORDER):
              room = UC_LURB;
              break;
            case (UPPER_BORDER | RIGHT_BORDER | BOTTOM_BORDER):
              room = UC_URBB;
              break;
            case (RIGHT_BORDER | BOTTOM_BORDER | LEFT_BORDER):
              room = UC_RBLB;
              break;
            case (BOTTOM_BORDER | LEFT_BORDER | UPPER_BORDER):
              room = UC_BLUB;
              break;
            default:
              room = UC_EMPTY;
              break;
            }
          dstr_append (buf, room, strlen (room));
        }
      if (r < lab->rows_count - 1)
        dstr_append (buf, "\r\n", 2);
    }
}
