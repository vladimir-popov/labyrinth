#ifndef __RENDER__
#define __RENDER__

#include "art.h"
#include "game.h"
#include "laby.h"
#include "u8.h"
#include <time.h>

struct render
{
  /* The count of symbols by vertical of one room.  */
  const int laby_room_height;
  /* The count of symbols by horizontal of one room.  */
  const int laby_room_width;

  /* The count of visible chars by vertical. */
  const int game_screen_height;

  /* The count of visible chars by horizontal. */
  const int game_screen_width;

  /* Visible rooms by vertical */
  const int visible_rows;

  /* Visible room by horizontal */
  const int visible_cols;

  /* This is a padding to keep player always on the screen */
  int visible_rows_pad;
  int visible_cols_pad;

  u8buf buf;
};

#define render_create(lh, lw, gh, gw)                                         \
  {                                                                           \
    (lh), (lw), (gh), (gw), (gh / lh), (gw / lw), 0, 0, U8_BUF_EMPTY          \
  }

#define DEFAULT_RENDER render_create (2, 4, 25, 78)

struct menu
{
  time_t last_update_at;
  int state;
};

#define MENU_EMPTY                                                            \
  {                                                                           \
    0.0, 0                                                                    \
  }

#define PLAYER_Y(Prow) (Prow * laby_room_height + laby_room_height / 2)
#define PLAYER_X(Pcol) (Pcol * laby_room_width + laby_room_width / 2)

/* The terminal resolution in chars by vertical. */
extern int terminal_window_height;
/* The terminal resolution in chars by horizontal. */
extern int terminal_window_width;

/* Calculates padding of the rendered area */
void render_update_visible_area (Render *render, Player *player, int laby_rows,
                                 int laby_cols);

void render (Render *render, Game *game);

void render_laby (Render *render, Laby *lab);

void render_welcome_screen (Render *render, void *menu);

void render_pause_menu (Render *render, void *menu);

void render_winning (Render *render, Game *game);
#endif // __RENDER__
