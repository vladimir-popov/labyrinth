#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "dstr.c"
#include "game.h"
#include "term.h"

/* The count of symbols by vertical of one room.  */
static const int laby_room_rows = 2;

/* The count of symbols by horizontal of one room.  */
static const int laby_room_cols = 4;

static int
expect_borders (int border, char expected)
{
  return (border & expected) == expected;
}

static int
not_expect_borders (int border, char expected)
{
  return (border & expected) == 0;
}

static char *
get_corner (int border, int neighbor)
{
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && not_expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┏";
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "╋";
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER))
    return "┣";
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && expect_borders (neighbor, BOTTOM_BORDER))
    return "┳";
  if (expect_borders (border, LEFT_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┫";
  if (expect_borders (border, UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┻";
  if (expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && not_expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┏";
  if (not_expect_borders (border, LEFT_BORDER | UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER | BOTTOM_BORDER))
    return "┛";
  if (expect_borders (border, UPPER_BORDER)
      && expect_borders (neighbor, RIGHT_BORDER))
    return "┗";
  if (expect_borders (border, LEFT_BORDER)
      && expect_borders (neighbor, BOTTOM_BORDER))
    return "┓";
  if (expect_borders (border, UPPER_BORDER)
      && not_expect_borders (neighbor, RIGHT_BORDER))
    return "━";
  if (expect_borders (border, LEFT_BORDER)
      && not_expect_borders (neighbor, BOTTOM_BORDER))
    return "┃";

  return " ";
}

/**
 * Renders the labyrinth `lab` to the buffer `buf`.
 */
void
render_laby (laby *lab, dstr *buf)
{
  char *s;
  /* Render rooms from every row, plus one extra row for the bottom borders */
  for (int r = 0; r <= lab->rows_count; r++)
    {
      /* iterates over room height (only one line for the last extra row) */
      int n = (r < lab->rows_count) ? laby_room_rows : 1;
      for (int i = 0; i < n; i++)
        {
          /* Take a room from the row, plus one more for the right border */
          for (int c = 0; c <= lab->cols_count; c++)
            {
              int is_visited = laby_is_visited (lab, r, c);

              /* We will render left and upper borders at once.
               * To choose correct symbol for the corner we need to know a
               * neighbor. */
              int border = laby_get_border (lab, r, c);
              int neighbor = laby_get_border (lab, r - 1, c - 1);

              /* Iterate over columns of the single room
               * (only one symbol for the extra right room) */
              int k = (c < lab->cols_count) ? laby_room_cols : 1;
              for (int j = 0; j < k; j++)
                {
                  /* render the first row of the room */
                  if (i == 0)
                    {
                      s = (j == 0) ? get_corner (border, neighbor)
                          : (border & UPPER_BORDER) ? "━"
                          : (is_visited || laby_is_visited (lab, r - 1, c))
                              ? "·"
                              : " ";
                    }
                  /* render the content of the room (the second row) */
                  else
                    {
                      int is_border = (j == 0) && (border & LEFT_BORDER);

                      s = (is_border) ? "┃" : (is_visited) ? "·" : " ";
                    }
                  dstr_append (buf, s, strlen (s));
                }
            }
          if (r < lab->rows_count)
            dstr_append (buf, "\r\n", 2);
        }
    }
}

void
render (level *level)
{
  dstr buf = DSTR_EMPTY;
  clear_screen();
  render_laby (&level->lab, &buf);
  write (STDIN_FILENO, buf.chars, buf.length);
  dstr_free(&buf);
}
