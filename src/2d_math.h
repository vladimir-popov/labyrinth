#ifndef __2D_MATH__
#define __2D_MATH__

#define pseudoscalar_product(l1x, l1y, l2x, l2y) (l1x * l2y - l1y * l2x)

#define vector_pseudoscalar_product(l1, l2)                                   \
  pseudoscalar_product (l1.px, l1.py, l2.px, l2.py)

typedef struct
{
  double x, y;
} Point;

#define new_point(x, y)                                                       \
  {                                                                           \
    (x), (y)                                                                  \
  }

typedef struct
{
  Point p0;
  Point p1;
} Line;

#define new_line(x0, y0, x1, y1)                                              \
  {                                                                           \
    new_point (x0, y0), new_point (x1, y1)                                    \
  }

typedef struct
{
  Point v;
  /* X projection */
  double px;
  /* Y projection */
  double py;
} Vector;

#define new_vector(x0, y0, px, py)                                            \
  {                                                                           \
    new_point (x0, y0), px, py                                                \
  }

void vector_by_points (Vector *v, Point from, Point to);

_Bool line_is_parallel (Line *l1, Line *l2);

_Bool line_is_projections_intersected (Line *l1, Line *l2);

/**
 * Checks that the line l1 is intersected with l2.
 */
_Bool line_is_intersected (Line *l1, Line *l2);

#endif // __2D_MATH__
