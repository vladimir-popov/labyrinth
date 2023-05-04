#include "term.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define DEBUG

struct termios orig_termios;

void
fatal (char *message)
{
  clear_screen ();
  fprintf (stderr, "Fatal error: %s\n", message);
  exit (FATAL_EXIT_CODE);
}

key_p
read_key ()
{
  char chars[3];
  key_p k = { 0, chars };
  k.len = read (STDIN_FILENO, chars, 1);
#ifndef DEBUG
  if (k.len < 0 && errno != EAGAIN)
    {
      char str[30];
      sprintf (str, "read key: errno=%d", errno);
      fatal (str);
    }
#endif

  if (chars[0] == ESC)
    {
      if (read (STDIN_FILENO, &chars[1], 1) == 1)
        k.len = 2;
      if (read (STDIN_FILENO, &chars[2], 1) == 1)
        k.len = 3;
    }
  return k;
}

void
disable_raw_mode ()
{
  if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    fatal ("recover termios");
}

void
enter_safe_raw_mode ()
{
  /* Remember terminal configuration and reset it on exit */
  if (tcgetattr (STDIN_FILENO, &orig_termios) < 0)
    fatal ("can't get tty settings");
  if (atexit (disable_raw_mode) != 0)
    fatal ("can't register reset of the terminal settings");

  struct termios raw;

  tcgetattr (STDIN_FILENO, &raw);

  /*
   * Remove some Local flags:
   *  ECHO   - used to not show printed symbol;
   *  ICANON - used to read input byte-by-byte, instead of line-byline;
   *  ISIG   - used to handle SIGINT (Ctrl-C) and SIGTSTP (Ctrl-Z) signals,
   *           here we do not exclude them for safety;
   *  IEXTEN - used to tell to the terminal to wait an another character
   *           (Ctrl-V);
   */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);

  /*
   * Remove some Input flags:
   *  BRKINT - turns on break condition;
   *  ICRNL  - used to translate '\r' to '\n';
   *  INPCK  - enables parity checking, which doesn’t seem to apply to modern
   *           terminal emulators;
   *  ISTRIP - causes the 8th bit of each input byte to be stripped, meaning
   *           it will set it to 0. This is probably already turned off;
   *  IXON   - it's for handle XOFF (Ctrl-S) and XON (Ctrl-Q) signals, which
   *           are used for software control and not actual for today;
   */
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  /*
   * Remove some Output flags:
   *  OPOST - used to translate '\n' to '\r\n';
   */
  // raw.c_oflag &= ~(OPOST);

  /* Set the character size to 8 bits per byte. */
  raw.c_cflag |= (CS8);

  /*
   * The VMIN value sets the minimum number of bytes
   * of input needed before read() can return
   */
  raw.c_cc[VMIN] = 0;

  /*
   * The VTIME value sets the maximum amount of time
   * to wait before read() returns.
   * It is in tenths of a second.
   * So 1 is equals to 1/10 of a second, or 100 milliseconds.
   */
  raw.c_cc[VTIME] = 1;

  if (tcsetattr (STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    fatal ("tcsetattr");
}

void
clear_screen ()
{
  /* Put cursor to the left upper corner */
  write (STDIN_FILENO, CUP, 3);
  /* Erase all lines on the screen */
  write (STDIN_FILENO, ED_FROM_START, 4);
}

void
hide_cursor ()
{
  write (STDIN_FILENO, RM_HIDE_CU, 6);
  if (atexit (show_cursor) != 0)
    fatal ("can't register reset visability of the cursor");
}

void
show_cursor ()
{
  write (STDIN_FILENO, SM_SHOW_CU, 6);
}

int
get_cursor_position (int *rows, int *cols)
{
  if (write (STDOUT_FILENO, DSR_GET_POSISION, 4) != 4)
    return -1;

  char buf[32];
  unsigned int i = 0;

  while (i < sizeof (buf) - 1)
    {
      if (read (STDIN_FILENO, &buf[i], 1) != 1)
        break;
      if (buf[i] == 'R')
        break;
      i++;
    }
  buf[i] = '\0';
  if (buf[0] != '\x1b' || buf[1] != '[')
    return -1;
  if (sscanf (&buf[2], "%d;%d", rows, cols) != 2)
    return -1;
  return 0;
}

int
get_window_size (int *rows, int *cols)
{
  struct winsize ws;
  if (ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
    {
      if (write (STDOUT_FILENO, CU_DOWN (999) CU_RIGHT (999), 12) != 12)
        return -1;

      return get_cursor_position (rows, cols);
    }
  else
    {
      *cols = ws.ws_col;
      *rows = ws.ws_row;
      return 0;
    }
}
