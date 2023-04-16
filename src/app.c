#include "uniclaby.c"
#include <time.h>

int
main (int argc, char *argv[])
{
  dstr buf = DSTR_EMPTY;
  time_t seed = time(NULL);
  laby lab = laby_generate_eller (5, 5, seed);
  // laby_print (&lab);
  uc_render_laby_compact (&buf, &lab);
  printf ("%s", buf.chars);
  return 0;
}
