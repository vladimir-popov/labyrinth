#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "term.h"
#include "rtmterm.c"

time_t seed = 0;
/* windows size in chars */
int height = 0;
int width = 0;


static int
parse_args (int argc, char *argv[])
{
  if (get_window_size (&height, &width) == -1)
    fatal ("Fail on getting the size of the window");

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
          width = strtol (optarg, NULL, 10);
          break;
        case 'h':
          height = strtol (optarg, NULL, 10);
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

  int rows = (height - 1) / laby_room_rows;
  int cols = (width - 1) / laby_room_cols;

  enter_safe_raw_mode ();
  clear_screen();
  hide_cursor ();

  game game;
  game_init(&game, rows, cols, seed);
  game_loop (&game);

  clear_screen ();
  return 0;
}
