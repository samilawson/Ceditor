#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

/* Disable raw mode at exit */
void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
/* Turn off echoing */
void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    /* Turns off Ctrl-S and Ctrl-Q, fix ctrl-M  */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* Turn off all output processing */
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    /* Turns off canonical mode, which reads input byte by byte instead of line by line */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main()
{
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
    {
        if (iscntrl(c))
        {
            printf("%d\r\n", c, c);
        }
        else
        {
            printf("%d ('%c')\r\n", c, c);
        }
    }
    return 0;
}