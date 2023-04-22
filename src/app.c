#include "uniclaby.c"
#include <time.h>

int
main (int argc, char *argv[])
{
  dstr buf = DSTR_EMPTY;
  time_t seed = time (NULL);

  printf ("Seed: %ld\r\n", seed);

  laby lab = laby_generate_eller (5, 7, seed);
  laby_print_borders (&lab);
  printf ("\n\n");
  laby_print_values (&lab);
  dstr_append (&buf, "\r\n\r\n", 4);
  uc_render_laby (&buf, &lab, 1, 2);
  printf ("%s", buf.chars);
  return 0;
}
