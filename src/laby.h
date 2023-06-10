#ifndef __LABY__
#define __LABY__

#include "2d_math.h"

/**
 * Coordinates of the room in the labyrinth.
 */
typedef struct
{
  int row;
  int col;
} LPos;

#define POSITION_EMPTY                                                        \
  {                                                                           \
    0, 0                                                                      \
  }

/*
 * All information about a single room should be encoded in one byte:
 *
 * |   C   |M|V|L U R B|
 * |0 ... 0|0|0|0 0 0 0|
 *
 * Section B: describes the bottom border of the room;
 * Section R: describes the right border of the room;
 * Section U: describes the upper border of the room;
 * Section L: describes the left border of the room;
 * Section V: was the room visited by the player or not;
 * Section M: is visible the room on map or not;
 * Section C: describes a content of the room;
 */
typedef unsigned int room;

typedef room *p_room;

#define VISITED_MASK 0x10
#define ON_MAP_MASK 0x11

#define VISIBILITY_SHIFT 4
#define ON_MAP_SHIFT 5
#define CONTENT_SHIFT 6

#define ROOM_IS_VISITED(P_ROOM) (*P_ROOM & VISITED_MASK)
#define ROOM_MARK_AS_VISITED(P_ROOM) (*P_ROOM |= VISITED_MASK)
#define ROOM_MARK_AS_NOT_VISITED(P_ROOM) (*P_ROOM &= ~VISITED_MASK)

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

/* Frees memory of the labyrinth. */
void laby_free (Laby *lab);

/*  Returns only 4 first bits, which are about borders of the room. */
unsigned char laby_get_borders (const Laby *lab, int y, int x);

/* Add border flag. */
void laby_add_border (Laby *lab, int y, int x, enum border border);

/* Remove border flag. */
void laby_rm_border (Laby *lab, int y, int x, enum border border);

Line laby_get_border_line (Laby *lab, int r, int c, enum border border,
                           int room_height, int room_width);

/**
 * Finds all visible rooms from the room on fy:fx in the range and put them to
 * the dest. Returns a count of the visible rooms (the length of the dest).
 */
int laby_find_visible_rooms (const Laby *lab, p_room **dest, int fy, int fx,
                             int range);

int laby_is_visited (const Laby *lab, int y, int x);

void laby_set_visited (Laby *lab, int y, int x);

void laby_set_content (Laby *lab, int y, int x, unsigned char value);

unsigned char laby_get_content (const Laby *lab, int y, int x);

void laby_generate (Laby *lab, int height, int width, int seed);

#endif /* __LABY__ */
