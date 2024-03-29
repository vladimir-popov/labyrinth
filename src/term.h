#ifndef __TERM_H__
#define __TERM_H__

#define FATAL_EXIT_CODE 42

/* Code of some keys */
#define KB_ENTER 13
#define KB_ESC 27
#define KB_SPACE 32
#define KB_BACKSPACE 127
#define KB_UP 'A'
#define KB_DOWN 'B'
#define KB_LEFT 'D'
#define KB_RIGHT 'C'

/* Control Symbol Introducer */
#define ESC '\x1b'
#define CSI "\x1b["

/* ED - Erase Display */
#define ED_FROM_START CSI "J"
#define ED_TO_END CSI "1J"
#define ED_FULL CSI "2J"

/* SM – Set Mode */
#define SM_SHOW_CU CSI "?25h"

/* RM – Reset Mode */
#define RM_HIDE_CU CSI "?25l"

/* CUP – Cursor Position */
#define CUP CSI "H"

/* Cursor Control */
#define CU_RIGHT(N) CSI #N "C"
#define CU_DOWN(N) CSI #N "B"

/* DSR – Device Status Report */
#define DSR_GET_POSISION CSI "6n"

/* SGR - Select Graphic Rendition */

/* Text colors */
#define SGR_RESET CSI "m"
#define SGR_BLACK CSI "30m"
#define SGR_RED CSI "31m"
#define SGR_GREEN CSI "32m"
#define SGR_YELLOW CSI "33m"
#define SGR_BLUE CSI "34m"
#define SGR_MAGENTA CSI "35m"
#define SGR_CYAN CSI "36m"
#define SGR_WHITE CSI "37m"

/* Text decoration */
#define SGR_BOLD CSI "1m"
#define SGR_UNDERLINE CSI "4m"
#define SGR_REVERSED CSI "7m"
#define SGR_INVERT_COLORS CSI "7m"

#define bold(str) SGR_BOLD str SGR_RESET

typedef struct
{
  int len;
  char *chars;
} KB_Code;

void fatal (char *message);

int is_key_pressed ();

KB_Code read_pressed_key ();

void disable_raw_mode ();

void enter_safe_raw_mode ();

void clear_screen ();

void hide_cursor ();

void show_cursor ();

int get_cursor_position (int *rows, int *cols);

/**
 * Allocates a memory for the string "CSI{rows};{cols}H\0" to the `dest` and
 * returns the length of the string as result. Or -1 in case of some errors.
 */
int set_cursor_position (char **dest, int rows, int cols);

int get_window_size (int *rows, int *cols);

/**
 *  Set up terminal resize callback.
 *  See `sigaction`.
 */
int handle_windows_resize (void (*handler) (int));

#endif /* __TERM_H__ */
