#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "rtmterm.c"
#include "term.h"

time_t seed = 0;

void
refresh_screen (int sig)
{
  if (get_window_size (&screen_rows, &screen_cols) == -1)
    fatal ("Fail on getting the size of the window");
  clear_screen ();
}

static int
parse_args (int argc, char *argv[])
{
  seed = time (NULL);

  int p;
  while ((p = getopt (argc, argv, "s:w:h:")) != -1)
    {
      switch (p)
        {
        case 's':
          seed = strtol (optarg, NULL, 10);
          break;
        case 'w':
          screen_cols = strtol (optarg, NULL, 10);
          break;
        case 'h':
          screen_rows = strtol (optarg, NULL, 10);
          break;
        case '?':
          if (optopt == 'h')
            fprintf (stderr, "The -h argument should be followed by a count "
                             "of symbols for whole labyrinth vertically.");
          else if (optopt == 'w')
            fprintf (stderr, "The -w argument should be followed by a count "
                             "of symbols for whole labyrinth horizontally.");
          else if (optopt == 's')
            fprintf (stderr, "The -s argument should be followed by a number, "
                             "which will be used as seed.");
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

  int height = (game_rows - 1) / laby_room_rows;
  int width = (game_cols - 1) / laby_room_cols;

  game game = GAME_EMPTY;
  game_init (&game, height, width, seed);
  game_loop (&game);

  clear_screen ();
  return 0;
}
