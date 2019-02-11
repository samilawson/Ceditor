/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** data ***/
struct termios orig_termios;

/*** terminal ***/
/* Error handling */
void die(const char *s)
{
    perror(s);
    exit(1);
}

/* Disable raw mode at exit */
void disableRawMode()
{
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die('tcsetattr');
}
/* Turn off echoing */
void enableRawMode()
{
    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    /* Turns off Ctrl-S and Ctrl-Q, fix ctrl-M  */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* Turn off all output processing */
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    /* Turns off canonical mode, which reads input byte by byte instead of line by line */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* Add a timeout for read */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

/*** init ***/
int main()
{
    enableRawMode();

    char c;
    while (1)
    {
        char c = '\0';
        if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if (iscntrl(c))
        {
            printf("%d\r\n", c, c);
        }
        else
        {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q')
            break;
    }
    return 0;
}