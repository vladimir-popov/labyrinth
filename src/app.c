#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include "game.h"
#include "term.h"
#include "render.c"

time_t seed = 0;
int height = 0;
int width = 0;

command
read_command ()
{
  int nread;
  char c;
  while ((nread = read (STDIN_FILENO, &c, 1)) != 1)
    {
      if (nread == -1 && errno != EAGAIN)
        fatal ("read");
    }

  if (c == ESC)
    {
      char seq[3];
      if (read (STDIN_FILENO, &seq[0], 1) != 1)
        return CMD_EXIT;
      if (read (STDIN_FILENO, &seq[1], 1) != 1)
        return c;
      if (seq[0] == '[')
        {
          switch (seq[1])
            {
            case 'A':
              return CMD_MV_UP;
            case 'B':
              return CMD_MV_DOWN;
            case 'C':
              return CMD_MV_RIGHT;
            case 'D':
              return CMD_MV_LEFT;
            }
        }
    }
  return c;
}

void
game_loop (level *level)
{
  command cmd;
  do
    {
      render (level);
      cmd = read_command ();
    }
  while (handle_command (level, cmd));
}

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

int
main (int argc, char *argv[])
{
  if (parse_args (argc, argv) != 0)
    return -1;

  int rows = (height - 1) / laby_room_rows;
  int cols = (width - 1) / laby_room_cols;

  enter_safe_raw_mode();
  hide_cursor();

  level level = new_level (rows, cols, seed);
  game_loop (&level);

  clear_screen();
  return 0;
}
