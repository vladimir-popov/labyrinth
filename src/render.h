#ifndef __RENDER__
#define __RENDER__

#include "art.h"
#include "laby.h"
#include "u8.h"
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
 * Prepare the labyrinth `lab` to render it.
 */
void render_laby (u8buf *buf, Laby *lab);

void render_welcome_screen (u8buf *buf, void *menu);

void render_pause_menu (u8buf *buf, void *menu);
#endif // __RENDER__
