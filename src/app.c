#include "uniclaby.c"

int
main (int argc, char *argv[])
{
  dstr buf = DSTR_EMPTY;
  laby lab = laby_generate_eller (5, 5, 3);
  // laby_print (&lab);
  uc_render_laby (&buf, &lab);
  printf ("%s", buf.chars);
  return 0;
}
