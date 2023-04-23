#include "term.c"
#include "uniclaby.c"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
  int width;
  int height;
  if (get_window_size (&height, &width) == -1)
    fatal ("Fail on getting the size of the window");

  int rows = 2;
  int cols = 4;

  time_t seed = time (NULL);

  int info = 0;

  int p;
  while ((p = getopt (argc, argv, "r:c:s:w:h:i")) != -1)
    {
      switch (p)
        {
        case 'r':
          rows = strtol (optarg, NULL, 10);
          break;
        case 'c':
          cols = strtol (optarg, NULL, 10);
          break;
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
          if (optopt == 'r')
            fprintf (stderr, "The -r argument should be followed by a count "
                             "of symbols for a single room vertically.");
          else if (optopt == 'c')
            fprintf (stderr, "The -c argument should be followed by a count "
                             "of symbols for a single room horizontaly.");
          else if (optopt == 's')
            fprintf (stderr, "The -s argument should be followed by a number, "
                             "which will be used as seed.");
          else
            fprintf (stderr, "Unknown option character '%c'.\n", optopt);
          return -1;
        }
    }

  dstr buf = DSTR_EMPTY;

  // enter_safe_raw_mode ();
  laby lab
      = laby_generate_eller ((height - info - 2) / rows, width / cols, seed);
  uc_render_laby (&buf, &lab, rows, cols);
  printf ("%s", buf.chars);

  if (info)
    printf ("\r\nLabyrint size: %dx%d\tRoom size: %dx%d\tSeed: %ld\r\n",
            height, width, rows, cols, seed);

  return 0;
}
