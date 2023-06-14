#ifndef __LABY__
#define __LABY__

#include "2d_math.h"

/*
 * All information about a single room should be encoded in one byte:
 *
 * |   C   |N|V|L U R B|
 * |0 ... 0|0|0|0 0 0 0|
 *
 * Section B: describes the bottom border of the room;
 * Section R: describes the right border of the room;
 * Section U: describes the upper border of the room;
 * Section L: describes the left border of the room;
 * Section V: is the room visible or not;
 * Section N: was the room visible by the player or not;
 * Section C: describes a content of the room;
 */
typedef unsigned int room;

typedef room *p_room;

/**
 * Checks the flag `expected` in the `border` and returns TRUE
 * if the flag IS set, or else FALSE.
 */
#define EXPECT_BORDERS(BORDERS, EXPECTED)                                     \
  ((BORDERS & (EXPECTED)) == (EXPECTED))

/**
 * Checks the flag `unexpected` in the `borders` and returns TRUE
 * if the flag is NOT set, or else FALSE.
 */
#define NOT_EXPECT_BORDERS(BORDERS, UNEXPECTED) ((BORDERS & (UNEXPECTED)) == 0)

enum border
{
  BOTTOM_BORDER = 1,
  RIGHT_BORDER = 2,
  UPPER_BORDER = 4,
  LEFT_BORDER = 8,
};

/* The objects which can be in the labyrinth */
enum content
{
  C_NOTHING = 0,
  C_PLAYER = 1,
  C_EXIT = 2,
};

/* The single horizontal line of rooms. */
typedef room *row;

/* The structure described a labyrinth */
typedef struct
{
  /* The count of rooms by horizontal */
  int cols;

  /* The count of rooms by vertical */
  int rows;

  /* The rows of the labyrinth's rooms */
  row *rooms;
} Laby;

/* Creates a new labyrinth with height x width empty rooms. */
void laby_init_empty (Laby *lab, int height, int width);

void laby_generate (Laby *lab, int height, int width, int seed);

/* Frees memory of the labyrinth. */
void laby_free (Laby *lab);

/*  Returns only 4 first bits, which are about borders of the room. */
unsigned char laby_get_borders (const Laby *lab, int y, int x);

/* Add border flag. */
void laby_add_border (Laby *lab, int y, int x, enum border border);

/* Remove border flag. */
void laby_rm_border (Laby *lab, int y, int x, enum border border);

/**
 * Calculates visibility of rooms from the center of the room r:c in the
 * labyrinth with 3x3 room size.
 * Here we use the similar to ray-trace idea:
 * - casts a glance in all possible directions from the center of the room r:c;
 * - marks as visible all rooms, which center can be achieved by the glance
 *   in the range without intersection with borders.
 *
 * @r zero based vertical position of the room from which visibility
 * calculated.
 * @c zero based horizontal position of the room from which visibility
 * calculated.
 * @range the maximum count of the visible rooms in a single direction.
 */
void laby_mark_visible_rooms (Laby *lab, int r, int c, int range);

_Bool laby_is_visible (const Laby *lab, int r, int c);

void laby_set_visibility (Laby *lab, int r, int c, _Bool flag);

_Bool laby_is_known_room (const Laby *lab, int r, int c);

void laby_mark_as_known (Laby *lab, int r, int c);

void laby_set_content (Laby *lab, int r, int c, enum content value);

unsigned char laby_get_content (const Laby *lab, int r, int c);

#endif /* __LABY__ */
