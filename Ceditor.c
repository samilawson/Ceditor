/*** includes ***/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/
#define CTRL_KEY(k) ((k)&0x1f)

/*** data ***/
struct termios orig_termios;

/*** terminal ***/
/* Error handling */
void die(const char *s)
{
    write(STDOUT_FILENO, "\x1b[2j", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

/* Disable raw mode at exit */
void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die('tcsetattr');
}
/* Turn off echoing */
void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
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
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}
/* Refactor keyboard input */
char editorReadKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }
    return c;
}
/*** output ***/
void editorRefreshScreen(){
    write(STDOUT_FILENO, "\x1b[2j", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}
/*** input ***/
void editorProcessKeypress()
{
    char c = editorReadKey();

    switch (c)
    {
        case CTRL_KEY('q');
        write(STDOUT_FILENO, "\x1b[2j", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;
    }
}

/*** init ***/
int main()
{
    enableRawMode();

    char c;
    while (1)
    {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
}