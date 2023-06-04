#ifndef __2D_MATH__
#define __2D_MATH__

#define pseudoscalar_product(ax, ay, bx, by) (ax * by - ay * bx)

#define vector_pseudoscalar_product(a, b)                                     \
  pseudoscalar_product (a.px, a.py, b.px, b.py)

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

void line_init (Line *l, double x0, double y0, double x1, double y1);

/**
 * Checks that the line a is intersected with b.
 */
_Bool line_is_intersected (Line *a, Line *b);

/**
 * Checks that the line AB is intersected with CD.
 */
_Bool is_lines_intersected (double Ax, double Ay, double Bx, double By,
                           double Cx, double Cy, double Dx, double Dy);

#endif // __2D_MATH__
