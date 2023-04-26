#ifndef __TERM_H__
#define __TERM_H__

#define FATAL_EXIT_CODE 42

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

void
fatal (char *message);

void
disable_raw_mode ();

void
enter_safe_raw_mode ();

void
clear_screen();

void
hide_cursor ();

void
show_cursor ();

int
get_cursor_position (int *rows, int *cols);

int
get_window_size (int *rows, int *cols);

#endif /* __TERM_H__ */
