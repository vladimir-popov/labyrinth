/**
 *  We will "draw" a game to the two dimension byte array as
 * indexes of the array with utf-8 symbols. It should help to avoid issues with
 * different size of symbols in utf-8 encoding. And at the end, we will
 * "render" this symbols map to the u8 buffer.
 */
#ifndef __RENDER__
#define __RENDER__

#include "art.h"
#include "u8.h"
#include "laby.h"
#include <time.h>


/* The terminal resolution in chars by vertical. */
extern int screen_rows;
/* The terminal resolution in chars by horizontal. */
extern int screen_cols;

/* The count of symbols by vertical of one room.  */
extern const int laby_room_height;
/* The count of symbols by horizontal of one room.  */
extern const int laby_room_width;

/* The count of visible chars by vertical. */
extern const int game_window_rows;
/* The count of visible chars by horizontal. */
extern const int game_window_cols;

/* Index of the particular symbol in the array of symbols. */
typedef unsigned char symbol;

/**
 * The map of symbols. It's a two dimensions array with indexes of the symbols,
 * which are representation of the drawn game field.
 */
typedef struct
{
  int height;
  int width;
  symbol **symbols;
} smap;

struct menu
{
  time_t last_update_at;
  int state;
};

#define MENU_EMPTY                                                            \
  {                                                                           \
    0.0, 0                                                                    \
  }

/**
 * Initialize the symbols map with enough symbols to "draw" a labyrinth with
 * `rows` x `cols` rooms, where a room has `room_height` x `room_width` symbols
 * dimension.
 */
void symbols_map_init (smap *sm, int rows, int cols, int room_height,
                       int room_width);

void symbols_map_free (smap *sm);

void draw_in_the_middle_of_room (smap *sm, int r, int c, symbol s);

/**
 * Prepare the labyrinth `lab` to render it.
 */
void draw_laby (smap *sm, Laby *lab);

/**
 * The similar to ray-trace idea:
 * - cast a glance in all possible directions in the range;
 * - marks all visible symbols in the direction till the end of the range,
 *   or meeting the first not visible symbol
 *
 * "y zero based vertical position of the view point in the symbols map.
 * @x zero based horizontal position of the view point in the symbols map.
 * @range the maximum count of the visible symbols in single direction.
 */
void draw_visible_area (smap *sm, Laby *lab, int y, int x, int range);

void render_symbols_map (U8_Buf *dest, const smap *source);

void render_welcome_screen (U8_Buf *buf, void *menu);

void render_pause_menu (U8_Buf *buf, void *menu);

void render_winning (U8_Buf *buf, void *menu);
#endif // __RENDER__
