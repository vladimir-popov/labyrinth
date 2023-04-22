#include "uniclaby.c"
#include <time.h>

int
main (int argc, char *argv[])
{
  dstr buf = DSTR_EMPTY;
  time_t seed = time(NULL);
  laby lab = laby_generate_eller (10, 15, seed);
  laby_print_borders (&lab);
  printf("\n\n");
  uc_render_laby_compact(&buf, &lab);
  dstr_append(&buf, "\r\n\r\n", 4);
  uc_render_laby (&buf, &lab, 1, 2);
  printf ("%s", buf.chars);
  return 0;
}
