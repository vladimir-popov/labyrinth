#include "2d_math.h"
#include <math.h>

void
vector_by_points (Vector *v, Point from, Point to)
{
  v->v.x = to.x;
  v->v.y = to.y;
  v->px = fmin (from.x, to.x) ? to.x - from.x : from.x - to.x;
  v->py = fmin (from.y, to.y) ? to.y - from.y : from.y - to.y;
}

void
line_init (Line *l, double x0, double y0, double x1, double y1)
{
  l->p0.x = x0;
  l->p0.y = y0;
  l->p1.x = x1;
  l->p1.y = y1;
}

static inline double
line_slope (Line *l)
{
  return (l->p1.y - l->p0.y) / (l->p1.x - l->p0.x);
}

_Bool
line_is_parallel (Line *l1, Line *l2)
{
  return line_slope (l1) == line_slope (l2);
}

static _Bool
is_projection_intersected (double l1p0, double l1p1, double l2p0, double l2p1)
{
  double l1s = fmin (l1p0, l1p1);
  double l2s = fmin (l2p0, l2p1);
  double l1e = fmax (l1p0, l1p1);
  double l2e = fmax (l2p0, l2p1);
  return fmax (l1s, l2s) <= fmin (l1e, l2e);
}

_Bool
line_is_projections_intersected (Line *l1, Line *l2)
{
  return is_projection_intersected (l1->p0.x, l1->p1.x, l2->p0.x, l2->p1.x)
         && is_projection_intersected (l1->p0.y, l1->p1.y, l2->p0.y, l2->p1.y);
}

/**
 * Checks is two lines are intersected.
 */
_Bool
line_is_intersected (Line *l1, Line *l2)
{
  if (line_is_projections_intersected (l1, l2))
    return 1;

  if (line_is_parallel (l1, l2))
    return 0;

  /*
   * @a the line from A to B
   * @b the line from C to D
   */
  Vector AB;
  vector_by_points (&AB, l1->p0, l1->p1);
  Vector AD;
  vector_by_points (&AD, l1->p0, l2->p1);
  Vector AC; // will be used also as CA
  vector_by_points (&AC, l1->p0, l2->p0);
  Vector CD;
  vector_by_points (&CD, l2->p0, l2->p1);
  Vector CB;
  vector_by_points (&CB, l2->p0, l1->p1);

  double v1 = vector_pseudoscalar_product (AB, AD);
  double v2 = vector_pseudoscalar_product (AB, AC);

  double v3 = vector_pseudoscalar_product (CD, AC);
  double v4 = vector_pseudoscalar_product (CD, CB);

  return isless (v1 * v2, 0) && isless (v3 * v4, 0);
}
