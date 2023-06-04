#include "2d_math.h"
#include <math.h>

void
vector_init (Vector *v, Point from, Point to)
{
  v->v.x = to.x;
  v->v.y = to.y;
  v->px = (from.x < to.x) ? to.x - from.x : from.x - to.x;
  v->py = (from.y < to.y) ? to.y - from.y : from.y - to.y;
}

void
line_init (Line *l, double x0, double y0, double x1, double y1)
{
  l->p0.x = x0;
  l->p0.y = y0;
  l->p1.x = x1;
  l->p1.y = y1;
}

static _Bool
is_projection_intersected (double pa_0, double pa_1, double pb_0, double pb_1)
{
  double as = fmin (pa_0, pa_1);
  double bs = fmin (pb_0, pb_1);
  double ae = fmax (pa_0, pa_1);
  double be = fmax (pb_0, pb_1);
  return fmax (as, bs) < fmin (ae, be);
}

_Bool
line_is_projections_intersected (Line *a, Line *b)
{
  return is_projection_intersected (a->p0.x, a->p1.x, b->p0.x, b->p1.x)
         && is_projection_intersected (a->p0.y, a->p1.y, b->p0.y, b->p1.y);
}

/**
 * Checks is two lines are intersected.
 */
_Bool
line_is_intersected (Line *a, Line *b)
{
  if (line_is_projections_intersected (a, b))
    return 1;

  /* 
   * @a the line from A to B
   * @b the line from C to D
   */
  Vector AB;
  vector_init(&AB, a->p0, a->p1);
  Vector AD;
  vector_init(&AD, a->p0, b->p1);
  Vector AC; // will be used also as CA
  vector_init(&AC, a->p0, b->p0);
  Vector CD;
  vector_init(&CD, b->p0, b->p1);
  Vector CB;
  vector_init(&CB, b->p0, a->p1);

  double v1 = vector_pseudoscalar_product (AD, AB);
  double v2 = vector_pseudoscalar_product(AB, AC);

  double v3 = vector_pseudoscalar_product(AC, CD);
  double v4 = vector_pseudoscalar_product(CD, CB);

  return isless(v1 * v2, 0) && isless(v3 * v4, 0);
}
