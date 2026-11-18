#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disableRawMode(void) {
  tcsetattr(STDERR_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode(void) {
  // get the current terminal attributes
  tcgetattr(STDIN_FILENO, &orig_termios);
  // restore original settings when the program exits normally
  atexit(disableRawMode);

  struct termios raw = orig_termios;
  // turn off misc. flags in accordance with `raw mode` conventions
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  // turn off canonical mode, terminal echo, and disable ctrl-c and ctrl-z
  raw.c_lflag &= ~(ECHO | ICANON | ISIG);

  raw.c_cc[VMIN] = 0; // min bytes of input needed before read() can return to 0 (return as soon as there's any input to read)
  raw.c_cc[VTIME] = 1; // max amount of time to wait before read() returns (set to 100ms)

  // set the updated terminal attributes.
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(void) {
  enableRawMode();

  while (1) {
    char c = '\0';
    // read the stdin buffer continuously till we get a terminate character (q)
    read(STDIN_FILENO, &c, 1);
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
    if (c == 'q')
      break;
  }

  return 0;
}
