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
    /* Turns off canonical mode, which reads inout byte by byte instead of line by line */
    raw.c_lflag &= ~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main()
{
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
        ;
    return 0;
}