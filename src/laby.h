#ifndef __LABY__
#define __LABY__

/**
 * Coordinates of the room in the labyrinth.
 */
typedef struct
{
  int y;
  int x;
} pos;

#define POSITION_EMPTY                                                        \
  {                                                                           \
    0, 0                                                                      \
  }

/*
 * All information about a single room should be encoded in one byte:
 *
 * |   C   |L U R B|V|
 * |0 ... 0|0 0 0 0|0|
 *
 * Section C: describes a content of the room;
 * Section L: describes the left border of the room;
 * Section U: describes the upper border of the room;
 * Section R: describes the right border of the room;
 * Section B: describes the bottom border of the room;
 * Section V: is visible room or not;
 */
typedef unsigned int room;

typedef room *p_room;

#define ROOM_IS_VISIBLE(P_ROOM) (*P_ROOM & 1)
#define ROOM_MARK_AS_VISIBLE(P_ROOM) (*P_ROOM |= 1)
#define ROOM_MARK_AS_NOT_VISIBLE(P_ROOM) (*P_ROOM &= ~1)

/**
 * Checks the flag `expected` in the `border` and returns 0 if the flag is NOT
 * set, or else a value > 0.
 */
#define EXPECT_BORDERS(BORDERS, EXPECTED) ((BORDERS & (EXPECTED)) == (EXPECTED))

/**
 * Checks the flag `unexpected` in the `borders` and returns 0 if the flag IS
 * set, or else a value > 0.
 */
#define NOT_EXPECT_BORDERS(BORDERS, UNEXPECTED) ((BORDERS & (UNEXPECTED)) == 0)

#define BORDERS_BITS 4
#define VISIBILITY_BITS 1
#define CONTENT_SHIFT 5

enum border
{
  BOTTOM_BORDER = 1 << VISIBILITY_BITS,
  RIGHT_BORDER = 2 << VISIBILITY_BITS,
  UPPER_BORDER = 4 << VISIBILITY_BITS,
  LEFT_BORDER = 8 << VISIBILITY_BITS,
};

/* The single horizontal line of rooms. */
typedef room *row;

/* The structure described a labyrinth */
typedef struct
{
  /* The count of rooms by horizontal */
  int width;

  /* The count of rooms by vertical */
  int height;

  /* The rows of the labyrinth's rooms */
  row *rooms;
} laby;

#define LABY_EMPTY                                                            \
  {                                                                           \
    0, 0, NULL                                                                \
  }

/* Creates a new labyrinth with height x width empty rooms. */
void laby_init_empty (laby *lab, int height, int width);

/* Frees memory of the labyrinth. */
void laby_free (laby *lab);

/*  Returns only 4 first bits, which are about borders of the room. */
unsigned char laby_get_border (const laby *lab, int y, int x);

/* Add border flag. */
void laby_add_border (laby *lab, int y, int x, enum border border);

/* Remove border flag. */
void laby_rm_border (laby *lab, int y, int x, enum border border);

/**
 * Finds all visible rooms from the room on fy:fx in the range and put them to
 * the dest. Returns a count of the visible rooms (the length of the dest).
 */
int laby_find_visible_rooms (const laby *lab, p_room **dest, int fy, int fx,
                             int range);

int laby_is_visible (const laby *lab, int y, int x);

void laby_set_visible (laby *lab, int y, int x);

void laby_set_content (laby *lab, int y, int x, unsigned char value);

unsigned char laby_get_content (const laby *lab, int y, int x);

void laby_generate (laby *lab, int height, int width, int seed);

#endif /* __LABY__ */
