#include "game.h"
#include "render.h"
#include "term.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

time_t seed = 0;

/* The terminal resolution in chars by vertical. */
int terminal_window_height = 0;
/* The terminal resolution in chars by horizontal. */
int terminal_window_width = 0;

/* the room counts of the generated labyrinth */
static int laby_rows = 0;
static int laby_cols = 0;

void
refresh_screen (int sig)
{
  if (get_window_size (&terminal_window_height, &terminal_window_width) == -1)
    fatal ("Fail on getting the size of the window");
  clear_screen ();
}

static int
parse_args (int argc, char *argv[])
{
  seed = time (NULL);

  int p;
  while ((p = getopt (argc, argv, "s:r:c:")) != -1)
    {
      switch (p)
        {
        case 's':
          seed = strtol (optarg, NULL, 0);
          break;
        case 'r':
          laby_rows = strtol (optarg, NULL, 0);
          break;
        case 'c':
          laby_cols = strtol (optarg, NULL, 0);
          break;
        case '?':
          if (optopt == 's')
            fprintf (stderr, "The -s argument should be followed by a number, "
                             "which will be used as a seed.");
          else if (optopt == 'r')
            fprintf (stderr, "The -r argument should be followed by a count "
                             "of rooms in the labyrinth by vertical.");
          else if (optopt == 'c')
            fprintf (stderr, "The -c argument should be followed by a count "
                             "of rooms in the labyrinth by horizontal.");
          else
            fprintf (stderr, "Unknown option character '%c'.\n", optopt);
          return -1;
        }
    }
  return 0;
}

int
main (int argc, char *argv[])
{
  if (parse_args (argc, argv) != 0)
    return -1;

  enter_safe_raw_mode ();
  handle_windows_resize (refresh_screen);
  refresh_screen (0);
  hide_cursor ();

  Render render = DEFAULT_RENDER;

  seed = (seed > 0) ? seed : time (NULL);
  laby_rows = (laby_rows > 0) ? laby_rows : render.visible_rows;
  laby_cols = (laby_cols > 0) ? laby_cols : render.visible_cols;

  Game game;
  game_init (&game, laby_rows, laby_cols, seed);
  game_run_loop (&game, &render);

  clear_screen ();
  return 0;
}
