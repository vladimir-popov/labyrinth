#ifndef __2D_MATH__
#define __2D_MATH__

#define pseudoscalar_product(ax, ay, bx, by) (ax * by - ay * bx)
#define vector_pseudoscalar_product(a, b) pseudoscalar_product (a.px, a.py, b.px, b.py)

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

void vector_init (Vector *v, Point from, Point to);

double vector_pseudo_scalar_product (Vector *a, Vector *b);

_Bool line_is_intersected (Line *a, Line *b);

#endif // __2D_MATH__
