#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "term.c"
#include "game.h"
#include "unicode_render.c"

time_t seed = 0;
int height = 0;
int width = 0;
int info = 0;

static int
parse_args (int argc, char *argv[])
{
  if (get_window_size (&height, &width) == -1)
    fatal ("Fail on getting the size of the window");

  seed = time (NULL);

  int p;
  while ((p = getopt (argc, argv, "r:c:s:w:h:i")) != -1)
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
        case 'i':
          /* use count of additional lines from information as true */
          info = 1;
          break;
        case '?':
          if (optopt == 'h')
            fprintf (stderr, "The -h argument should be followed by a count "
                             "of symbols for whole labyrint vertically.");
          else if (optopt == 'w')
            fprintf (stderr, "The -w argument should be followed by a count "
                             "of symbols for whole labyrint horizontaly.");
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

key read_key () 
{
  return KEY_ESC;
}

void
render (game *game)
{
  dstr buf = DSTR_EMPTY;
  unicode_render (&game->level, &buf);
  printf ("%s", buf.chars);
}

int
main (int argc, char *argv[])
{
  if (parse_args (argc, argv) != 0)
    return -1;

  int rows = (height - info - 2) / laby_room_rows;
  int cols = width / laby_room_cols;

  game game = new_game(rows, cols, seed);
  game_loop(&game);

  if (info)
    printf ("\r\nLabyrint size: %dx%d\tSeed: %ld\r\n", rows, cols, seed);

  return 0;
}
