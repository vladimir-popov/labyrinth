#include "term.c"
#include "unicode_render.c"
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

  time_t seed = time (NULL);

  int info = 0;

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

  dstr buf = DSTR_EMPTY;

  // enter_safe_raw_mode ();
  laby lab
      = laby_generate_eller ((height - info - 2) / r_rows, width / r_cols, seed);
  laby_render (&buf, &lab);
  printf ("%s", buf.chars);

  if (info)
    printf ("\r\nLabyrint size: %dx%d\tSeed: %ld\r\n",
            height, width, seed);

  return 0;
}
