/* <| Shado :: a lightweight, modular, and modal editor |> */
// -- Imports -- {{{
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <asm-generic/errno-base.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
// }}}
// -- Macros -- {{{
#define SHADO_VERSION "0.0.1"
#define TAB_STOP 4
#define CTRL_KEY(k) ((k) & 0x1f)
//}}}
// -- Data -- {{{
typedef struct erow {
    int size;
    int rsize;
    char *chars;
    char *render;
} erow;

struct editorConfig {
    int cx, cy;
    int rx;
    int rowoff;
    int coloff;
    int screenrows;
    int screencols;
    int numrows;
    erow *row;
    struct termios orig_termios;
};

struct editorConfig E;

enum editorKey {
    LEFT = 'h',
    DOWN = 'j',
    UP = 'k',
    RIGHT = 'l',
    ARROW_LEFT = 1000,
    ARROW_DOWN,
    ARROW_UP,
    ARROW_RIGHT,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN,
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
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
            } else
                switch (seq[1]) {
                    case 'A': return LEFT;
                    case 'B': return UP;
                    case 'C': return DOWN;
                    case 'D': return RIGHT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
        } else if (seq[0] == '0')
            switch (seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
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
// -- Row Ops -- {{{
int editorRowCxToRx (erow *row, int cx) {
    int rx = 0;
    int i;
    for (i = 0; i < cx; i++) { 
         if (row->chars[i] == '\t')
             rx += (TAB_STOP - 1) - (rx % TAB_STOP);
         rx++;
    }
    return rx;
}

void editorUpdateRow (erow *row) {
    int tabs = 0;
    int j;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == '\t') tabs++;

    free(row->render);
    row->render = malloc(row->size + (tabs* (TAB_STOP - 1)) + 1);

    int idx = 0;
    for (j = 0; j < row->size; j++)
        if (row->chars[j] == '\t') {
            row->render[idx++] = row->chars[j];
            while (idx % TAB_STOP != 0) row->render[idx++] = ' ';
        } else
            row->render[idx++] = row->chars[j];

    row->render[idx] = '\0';
    row->rsize = idx;
}

void editorAppendRow(char *s, size_t len) {
    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));

    int at = E.numrows;
    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';

    E.row[at].rsize = 0;
    E.row[at].render = NULL;
    editorUpdateRow(&E.row[at]);
    
    E.numrows++;
}
//}}}
// -- File IO -- {{{
void editorOpen (char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) kill("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorAppendRow(line, linelen);
    }
    free(line);
    fclose(fp);
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
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

    switch (key) {
        case LEFT: case ARROW_LEFT:
            if (E.cx != 0)
                E.cx--;
            else if (E.cy > 0) {
                E.cy--;
                E.cx = E.row[E.cy].size;
            }
            break;
        case DOWN: case ARROW_DOWN:
            if (E.cy < E.numrows)
                E.cy++;
            break;
        case UP: case ARROW_UP:
            if (E.cy != 0)
                E.cy--;
            break;
        case RIGHT: case ARROW_RIGHT:
            if (row && E.cx < row->size)
                E.cx++;
            else if (row && E.cx == row->size) {
                E.cy++;
                E.cx = 0;
            }
            break;
    }

    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen)
        E.cx = rowlen;
}

void editorProcessKeypress () {
    int c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;

        case HOME_KEY:
            E.cx = 0;
            break;
        case END_KEY:
            if (E.cy < E.numrows)
                E.cx = E.row[E.cy].size;
            break;

        case PAGE_UP: case PAGE_DOWN:
            {
                if (c == PAGE_UP)
                    E.cy = E.rowoff;
                else if (c == PAGE_DOWN) {
                    E.cy = E.rowoff + E.screenrows - 1;
                    if (E.cy > E.numrows) E.cy = E.numrows;
                }

                int times = E.screenrows;
                while (times--)
                    editorMoveCursor(c == PAGE_UP ? UP : DOWN);
            }
            break;

        case LEFT: case DOWN: case UP: case RIGHT:
        case ARROW_LEFT: case ARROW_DOWN: case ARROW_UP: case ARROW_RIGHT:
            editorMoveCursor(c);
            break;
    }
}
//}}}
// -- Output -- {{{
void editorScroll () {
    E.rx = 0;
    if (E.cy < E.numrows)
        E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);

    if (E.cy < E.rowoff)
        E.rowoff = E.cy;
    if (E.cy >= E.rowoff + E.screenrows)
        E.rowoff = E.cy - E.screenrows + 1;
    if (E.rx < E.coloff)
        E.coloff = E.rx;
    if (E.rx >= E.coloff + E.screencols)
        E.coloff = E.rx - E.screencols + 1;
}

void editorDrawRows (struct abuf *ab) {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        int filerow = y + E.rowoff;
        if (filerow >= E.numrows) {
            if (E.numrows == 0 && y == E.screenrows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                        "Kilo editor -- version %s", SHADO_VERSION);
                if (welcomelen > E.screencols) welcomelen = E.screencols;
                int padding = (E.screencols - welcomelen) / 2;
                if (padding) {
                    abAppend(ab, "~", 1);
                    padding--;
                }
                while (padding--) abAppend(ab, " ", 1);
                abAppend(ab, welcome, welcomelen);
            } else
                abAppend(ab, "~", 1);
        } else {
            int len = E.row[filerow].rsize - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols) len = E.screencols;
            abAppend(ab, &E.row[filerow].render[E.coloff], len);
        }
        abAppend(ab, "\x1b[K", 3);
        if (y < E.screenrows - 1)
            abAppend(ab, "\r\n", 2);
    }
}

void editorRefreshScreen () {
    editorScroll();
    struct abuf ab = ABUF_INIT;
    abAppend(&ab, "\x1b[?25l", 6); // Hide Cursor
    abAppend(&ab, "\x1b[H", 3); // Reposition Cursor

    editorDrawRows(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1,
                                              (E.rx - E.coloff) + 1);
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
    E.rx = 0;
    E.numrows = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.row = NULL;
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) kill("getWindowSize");
}

int main (int argc, char *argv[]) {
    enterRawMode();
    initEditor();
    if (argc >= 2) editorOpen(argv[1]);

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
} //}}}
/* <| ------------------------------------------------- |> */
