#include "2d_math.h"
#include "minunit.h"

static char *
intersected_lines_test ()
{
  // given:
  // A   B
  //   X
  // C   D
  Point A = { 0.0, 0.0 };
  Point B = { 2.0, 0.0 };
  Point C = { 0.0, 1.0 };
  Point D = { 2.0, 1.0 };
  Line AD = new_line(A.x, A.y, D.x, D.y);
  Line CB = new_line(C.x, C.y, B.x, B.y);

  // when:
  _Bool res = line_is_intersected (&AD, &CB);

  // then:
  mu_assert ("Lines should intersect", res);
  return 0;
}

static char *
parallel_lines_intersection_test ()
{
  // given:
  // A------B
  //
  // C------D
  Point A = { 0.0, 0.0 };
  Point B = { 2.0, 0.0 };
  Point C = { 0.0, 1.0 };
  Point D = { 2.0, 1.0 };
  Line AB = new_line(A.x, A.y, B.x, B.y);
  Line CD = new_line(C.x, C.y, D.x, D.y);

  // when:
  _Bool is_parallel = line_is_parallel(&AB, &CD);
  _Bool res = line_is_intersected (&AB, &CD);

  // then:
  mu_assert("Lines are not parallel", is_parallel);
  mu_assert ("Parallel lines should not intersect", !res);
  return 0;
}

static char *
perpendicular_lines_intersection_test ()
{
  // given:
  // A
  // |
  // C------D
  Point A = { 0.0, 0.0 };
  Point C = { 0.0, 1.0 };
  Point D = { 2.0, 1.0 };
  Line CA = new_line(C.x, C.y, A.x, A.y);
  Line CD = new_line(C.x, C.y, D.x, D.y);

  // when:
  _Bool is_pr_int = line_is_projections_intersected(&CA, &CD);
  _Bool res = line_is_intersected (&CA, &CD);

  // then:
  mu_assert ("Projections of lines should intersect", is_pr_int);
  mu_assert ("Perpendicular lines should intersect", res);
  return 0;
}

static char *
lines_intersection_test_1 ()
{
  // given:
  //       A
  //       |
  //C---D  |
  //       |
  //       B
  Point A = { 16.0, 4.0 };
  Point B = { 16.0, 6.0 };
  Point C = { 11.0, 5.0 };
  Point D = { 12.0, 5.0 };
  Line AB = new_line(A.x, A.y, B.x, B.y);
  Line CD = new_line(C.x, C.y, D.x, D.y);

  // when:
  _Bool res = line_is_intersected (&AB, &CD);

  // then:
  mu_assert ("Lines should not intersect", !res);
  return 0;
}
