#include "uniclaby.c"
#include <time.h>

int
main (int argc, char *argv[])
{
  dstr buf = DSTR_EMPTY;
  time_t seed = time (NULL);

  printf ("Seed: %ld\r\n", seed);

  laby lab = laby_generate_eller (14, 15, seed);
  uc_render_laby (&buf, &lab, 2, 4);
  printf ("%s", buf.chars);
  return 0;
}
