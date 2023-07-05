#include "game.h"
#include "render.h"
#include "term.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define help_title(title) printf (bold (title) "\n")
#define help_descr(name, descr) printf (bold ("\t" name) " - " descr "\n")
#define help_option(opt, descr) printf (bold ("\t" opt "\t") descr "\n")

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
  while ((p = getopt (argc, argv, "h s: r: c:")) != -1)
    {
      switch (p)
        {
        case 'h':
          help_title ("NAME");
          help_descr ("labyrinth",
                      "a game about looking for exit from the labyrinth.");
          help_title ("OPTIONS");
          help_option (
              "-s", "an initial seed of the game. Used to generate levels.");
          help_option ("-r", "the rows count of the labyrinth.");
          help_option ("-c", "the cols count of the labyrinth.");
          // clang-format off
          help_title ("KEYS SETTINGS");
          printf( \
              "\t" bold ("?") " - show keys settings menu;\n" 
              "\t" bold (":") " - command mode;\n" 
              "\t" bold("Space") " or " bold ("m") " - toggle the map;\n" 
              "\t" bold ("ESC") " - put the game on pause;\n  \n" 
              "\t" bold("Moving:") " \n"
              "\t" bold ("↑") " or " bold ("j") " - move to the upper room;\n" 
              "\t" bold ( "↓") " or " bold ("k") " - move to the bottom room;\n"
              "\t" bold ( "←") " or " bold ("h") " - move to the left room;\n"
              "\t" bold ( "→") " or " bold ("l") " - move to the right room;\n"
              "\t" bold ( "→") " or " bold ("l") " - move to the right room;\n"
          );
          help_title("AUTHOR");
          printf("Vladimir Popov <vladimir@dokwork.ru>\n");
          // clang-format on
          return -1;
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
