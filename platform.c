/* Platform dependent code, (keyboard input!) */

#include "main.h"

#ifndef WIN32
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

// Global for "atexit()"
struct termios oldt, newt;
int oldfopts;

// Called by atexit() to reset terminal attributes
void reset_term(void)
{
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldfopts);
}


// POSIX emulation of DOS/Windows "_kbhit". (Non-blocking check if a character is waiting)
// Also buffers characters, because ungetc only buffers one byte, so we end up dropping characters.
int posix_kbhit(void)
{
	static int initialized_term = 0;
	int ch = 0;
	
	if (!initialized_term)
	{
		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt; // Save original attributes
		newt.c_lflag &= ~(ICANON | ECHO);
		tcsetattr(STDIN_FILENO, TCSANOW, &newt);
		oldfopts = fcntl(STDIN_FILENO, F_GETFL, 0); // Save original fopts
		fcntl(STDIN_FILENO, F_SETFL, oldfopts | O_NONBLOCK);
		initialized_term = 1;
		atexit(reset_term); // Make sure we reset terminal on exit
	}

	ch = getchar(); 
		
	if(ch != EOF) 
	{
		ungetc(ch, stdin); // If we actually did read a character, unread it
		return 1;
	}
	return 0;
}
#endif
