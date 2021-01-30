/* <| Shado :: a lightweight, modular, and modal editor |> */
// -- Imports -- {{{
#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
// }}}
// -- Macros -- {{{
#define SHADO_VERSION "0.0.1"
#define CTRL_KEY(k) ((k) & 0x1f)
//}}}
// -- Data -- {{{
struct editorConfig {
    int cx, cy;
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

struct editorConfig E;

enum editorKey {
    LEFT = 1000,
    DOWN,
    UP,
    RIGHT,
    PAGE_UP,
    PAGE_DOWN,
    ARROW_LEFT,
    ARROW_DOWN,
    ARROW_UP,
    ARROW_RIGHT,
};
// --- Append buffer -- {{{
struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT { NULL, 0 }
// }}}
//}}}
// -- Terminal -- {{{
void kill (const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void disableRawMode () { // Restores on exit to terminal's orig attributes
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        kill("tcsetattr");
}

void enterRawMode () {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) kill("tcgetarr");
    atexit(disableRawMode); // at exit, do ...

    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) kill("tcsetattr");
}

int editorReadKey () {
    int nread;
    char c;
    // loop: read 1B into `c` from stdin
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
        if (nread == -1 && errno != EAGAIN) kill("read");
    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~')
                    switch (seq[1]) {
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                    }
            } else
                switch (seq[1]) {
                    case 'A': return LEFT;
                    case 'B': return UP;
                    case 'C': return DOWN;
                    case 'D': return RIGHT;
                }
        }
        return '\x1b';
    } else
        return c;
}

int getCursorPosition (int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    while  (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    printf("\r\n&buf[1]: '%s'\r\n", &buf[1]);

    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

    return 0;
}

int getWindowSize (int *rows, int *cols) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return getCursorPosition(rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}
//}}}
// -- Append -- {{{
void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = realloc(ab->b, ab->len + len);

    if (new == NULL) return;
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab) {
    free(ab->b);
}
//}}}
// -- Input -- {{{
void editorMoveCursor (int key) {
    switch (key) {
        case LEFT:
            if (E.cx != 0)
                E.cx--;
            break;
        case DOWN:
            if (E.cy != E.screenrows - 1)
                E.cy++;
            break;
        case UP:
            if (E.cy != 0)
                E.cy--;
            break;
        case RIGHT:
            if (E.cx != E.screencols - 1)
                E.cx++;
            break;
    }
}

void editorProcessKeypress () {
    int c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
        case LEFT: case DOWN: case UP: case RIGHT:
            editorMoveCursor(c);
            break;
    }
}
//}}}
// -- Output -- {{{
void editorDrawRows(struct abuf *ab) {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        if (y == E.screenrows / 3) {
            char welcome[80];
            int welcomelen = snprintf(welcome, sizeof(welcome),
                    "Shado Editor -- version: %s", SHADO_VERSION);
            if (welcomelen > E.screencols) welcomelen = E.screencols;
            int padding = (E.screencols - welcomelen) / 2; // Center welcome msg
            if (padding) {
                abAppend(ab, "~", 1);
                padding--;
            }
            while (padding--) abAppend(ab, " ", 1);
            abAppend(ab, welcome, welcomelen);
        } else
            abAppend(ab, "~", 1);

        abAppend(ab, "\x1b[K", 3); // clear line when redrawn
        if (y < E.screenrows - 1)
            abAppend(ab, "\r\n", 2);
    }
}

void editorRefreshScreen () {
    struct abuf ab = ABUF_INIT;
    abAppend(&ab, "\x1b[?25l", 6); // Hide Cursor
    abAppend(&ab, "\x1b[H", 3); // Reposition Cursor

    editorDrawRows(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + 1);
    abAppend(&ab, buf, strlen(buf));
    abAppend(&ab, "\x1b[?25h", 6); // Unhide Cursor

    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}
//}}}
// -- Init main -- {{{
void initEditor () {
    E.cx = 0;
    E.cy = 0;
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) kill("getWindowSize");
}

int main () {
    enterRawMode();
    initEditor();

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
} //}}}
/* <| ------------------------------------------------- |> */
