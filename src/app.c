#include "game.h"
#include "render.h"
#include "term.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

time_t seed = 0;

/* The terminal resolution in chars by vertical. */
int screen_height = 0;
/* The terminal resolution in chars by horizontal. */
int screen_width = 0;

/* The count of symbols by vertical of one room.  */
const int laby_room_height = 2;
/* The count of symbols by horizontal of one room.  */
const int laby_room_width = 4;

/* The count of visible chars by vertical. */
const int game_window_height = 25;
/* The count of visible chars by horizontal. */
const int game_window_width = 78;

void
refresh_screen (int sig)
{
  if (get_window_size (&screen_height, &screen_width) == -1)
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
          screen_width = strtol (optarg, NULL, 10);
          break;
        case 'h':
          screen_height = strtol (optarg, NULL, 10);
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

  int height = (game_window_height - 1) / laby_room_height;
  int width = (game_window_width - 1) / laby_room_width;

  Game game;
  game_init (&game, height, width, seed);
  game_run_loop (&game);

  clear_screen ();
  return 0;
}
