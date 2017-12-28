/* Platform dependent code, (keyboard input!) */

#ifndef WIN32
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// POSIX emulation of DOS/Windows "_kbhit". (Non-blocking check if a character is waiting)
int posix_kbhit(void)
{
  struct termios oldt, newt;
  int ch, oldfopts;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldfopts = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldfopts | O_NONBLOCK);

  ch = getchar();

  // Reset terminal attributes
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldfopts);

  // If we actually read a character, unget it!
  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}
#endif

