#ifndef __RENDER__
#define __RENDER__

#include "art.h"
#include "game.h"
#include "laby.h"
#include "u8.h"
#include <time.h>

enum menu_option
{
  M_NEW_GAME,
  M_CONTINUE,
  M_EXIT
};

#define MAX_CMD_LENGTH 10

struct menu
{
  /* The current option in the menu */
  enum menu_option option;
  /* Used to accumulate user command from the command line */
  char *cmd;
};

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

  /* The actual count of visible rooms by vertical */
  const int visible_rows;

  /* The actual count of visible rooms by horizontal */
  const int visible_cols;

  /* This is a padding to keep player always on the screen */
  int visible_rows_pad;
  int visible_cols_pad;

  u8buf buf;
};

/**
 * Creates a new render.
 * @lh count of symbols in the room by vertical.
 * @lw count of symbols in the room by horizontal.
 * @gh count of symbols in the game window by vertical.
 * @gw count of symbols in the game window by horizontal.
 */
#define render_create(lh, lw, gh, gw)                                         \
  {                                                                           \
    (lh), (lw), (gh), (gw), (gh / lh), (gw / lw), 0, 0, U8_BUF_EMPTY          \
  }

#define DEFAULT_RENDER render_create (2, 4, 25, 78)

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

void render_laby (Render *render, Laby *lab, enum laby_draw_mode mode);

void render_welcome_screen (Render *render, Menu *menu);

void render_keys_settings (Render *render);

void render_pause_menu (Render *render, Menu *menu);

void render_cmd_menu (Render *render, Menu *menu);

void render_winning (Render *render, Game *game);
#endif // __RENDER__
