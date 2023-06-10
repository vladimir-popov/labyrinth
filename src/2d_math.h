#ifndef __2D_MATH__
#define __2D_MATH__

#define pseudoscalar_product(l1x, l1y, l2x, l2y) (l1x * l2y - l1y * l2x)

#define vector_pseudoscalar_product(l1, l2)                                   \
  pseudoscalar_product (l1.px, l1.py, l2.px, l2.py)

typedef struct
{
  double x, y;
} Point;

typedef struct
{
  Point p0;
  Point p1;
} Line;

typedef struct
{
  Point v;
  /* X projection */
  double px;
  /* Y projection */
  double py;
} Vector;

void vector_initp (Vector *v, Point from, Point to);

void line_init (Line *l, double x0, double y0, double x1, double y1);

_Bool line_is_parallel (Line *l1, Line *l2);

_Bool line_is_projections_intersected (Line *l1, Line *l2);

/**
 * Checks that the line l1 is intersected with l2.
 */
_Bool line_is_intersected (Line *l1, Line *l2);

_Bool line_is_intersectedp (Line *l, double Cx, double Cy, double Dx,
                           double Dy);

#endif // __2D_MATH__
