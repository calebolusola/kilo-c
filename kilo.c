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

  // set the updated terminal attributes
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(void) {
  enableRawMode();

  char c;
  // read the stdin buffer continuously till we get a terminate character (q)
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
    if (iscntrl(c)) {
      printf("%d\r\n", c);
    } else {
      printf("%d ('%c')\r\n", c, c);
    }
  }

  return 0;
}
