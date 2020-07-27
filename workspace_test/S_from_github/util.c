#include "util.h"

int key_read()
{
	int c;
	struct termios init;
	struct termios tmp;
	
	/* Read in current terminal state, then make a copy */
	tcgetattr(fileno(stdin), &init);
	memcpy(&tmp, &init, sizeof(struct termios));
	
	/* Change state to not echo the character and don't wait for newline */
	tmp.c_lflag &= ~(ECHO | ICANON);
	/*tmp.c_cc[VTIME] = 0;
	tmp.c_cc[VMIN] = 0;*/
	tcsetattr(fileno(stdin), TCSANOW, &tmp);
	
	/* Get character */
	c = fgetc(stdin);
	
	/* Restore terminal state */
	tcsetattr(fileno(stdin), TCSANOW, &init);
	
	return c;
}
