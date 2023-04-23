#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define FATAL_EXIT_CODE 42

/* Control Symbol Introducer */
#define ESC '\x1b'
#define ESCS "\x1b["

/* Cursor Control */
#define MOVE_RIGHT(N) ESCS #N "C"
#define MOVE_DOWN(N) ESCS #N "B"

/* DSR – Device Status Report */
#define DSR_GET_POSISION ESCS "6n"

/* SGR - Select Graphic Rendition */

/* Text colors */
#define SGR_RESET ESCS "m"
#define SGR_BLACK ESCS "30m"
#define SGR_RED ESCS "31m"
#define SGR_GREEN ESCS "32m"
#define SGR_YELLOW ESCS "33m"
#define SGR_BLUE ESCS "34m"
#define SGR_MAGENTA ESCS "35m"
#define SGR_CYAN ESCS "36m"
#define SGR_WHITE ESCS "37m"

/* Text decoration */
#define SGR_BOLD ESCS "1m"
#define SGR_UNDERLINE ESCS "4m"
#define SGR_REVERSED ESCS "7m"
#define SGR_INVERT_COLORS ESCS "7m"

struct termios orig_termios;

void
fatal (char *message)
{
  fprintf (stderr, "Fatal error: %s", message);
  exit (FATAL_EXIT_CODE);
}

void
disable_raw_mode ()
{
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &orig_termios);
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
  raw.c_cflag &= ~(ECHO | ICANON | IEXTEN);

  /*
   * Remove some Input flags:
   *  IXON   - it's for handle XOFF (Ctrl-S) and XON (Ctrl-Q) signals, which
   *           are used for software control and not actual for today;
   *  ICRNL  - used to translate '\r' to '\n';
   *  BRKINT - turns on break condition;
   *  INPCK  - enables parity checking, which doesn’t seem to apply to modern
   *           terminal emulators;
   *  ISTRIP - causes the 8th bit of each input byte to be stripped, meaning
   *           it will set it to 0. This is probably already turned off;
   */
  raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);

  /*
   * Remove some Output flags:
   *  OPOST - used to translate '\n' to '\r\n';
   */
  raw.c_oflag &= ~(OPOST);

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

  tcsetattr (STDIN_FILENO, TCSAFLUSH, &raw);
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
      if (write (STDOUT_FILENO, MOVE_DOWN (999) MOVE_RIGHT (999), 12) != 12)
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
