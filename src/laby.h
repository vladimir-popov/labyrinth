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
 * |   C   |M|V|L U R B|
 * |0 ... 0|0|0|0 0 0 0|
 *
 * Section B: describes the bottom border of the room;
 * Section R: describes the right border of the room;
 * Section U: describes the upper border of the room;
 * Section L: describes the left border of the room;
 * Section V: is visible the room for player or not;
 * Section M: is visible the room on map or not;
 * Section C: describes a content of the room;
 */
typedef unsigned int room;

typedef room *p_room;

#define VISIBILITY_MASK 0x10
#define ON_MAP_MASK 0x11

#define VISIBILITY_SHIFT 4
#define ON_MAP_SHIFT 5
#define CONTENT_SHIFT 6

#define ROOM_IS_VISIBLE(P_ROOM) (*P_ROOM & VISIBILITY_MASK)
#define ROOM_MARK_AS_VISIBLE(P_ROOM) (*P_ROOM |= VISIBILITY_MASK)
#define ROOM_MARK_AS_NOT_VISIBLE(P_ROOM) (*P_ROOM &= ~VISIBILITY_MASK)

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
