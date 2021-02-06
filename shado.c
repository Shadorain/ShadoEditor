/* <| Shado :: a lightweight, modular, and modal editor |> */
// -- Includes -- {{{
#include "shado.h"
// }}}
struct editorConfig E;
// -- Status Bar -- {{{
void editorDrawStatusBar (struct abuf *ab) {
    abAppend(ab, "\x1b[7m", 4);
    char status[80], rstatus[80];
    /* int len = snprintf(status, sizeof(status), "%.20s - %d lines", */
    /*         E.filename ? E.filename : "[No Name]", E.numrows); */
    int len = snprintf(status, sizeof(status), "%.20s %s",
            E.filename ? E.filename : "[No Name]",
            E.dirty ? "(modified)" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d:%d",
            E.syntax ? E.syntax->filetype : "no ft", E.cy + 1, E.numrows);
    if (len > E.screencols) len = E.screencols;
    abAppend(ab, status, len);
    while (len < E.screencols) {
        if (E.screencols - len == rlen) {
            abAppend(ab, rstatus, rlen);
            break;
        } else {
            abAppend(ab, " ", 1);
            len++;
        }
    }
    abAppend(ab, "\x1b[m", 3);
    abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar (struct abuf *ab) {
    abAppend(ab, "\x1b[K", 3);
    int msglen = strlen(E.stsmsg);
    if (msglen > E.screencols) msglen = E.screencols;
    if (msglen && time(NULL) - E.stsmsg_time < 5)
        abAppend(ab, E.stsmsg, msglen);
}

void editorSetStatusMessage (const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.stsmsg, sizeof(E.stsmsg), fmt, ap);
    va_end(ap);
    E.stsmsg_time = time(NULL);
}
//}}}
// -- File IO -- {{{
char *editorRowsToString (int *buflen) {
    int tlen = 0;
    int i;
    for (i = 0; i < E.numrows; i++)
         tlen += E.row[i].size + 1;
    *buflen = tlen;

    char *buf = malloc(tlen);
    char *p = buf;
    for (i = 0; i <E.numrows; i++) {
        memcpy(p, E.row[i].chars, E.row[i].size);
        p += E.row[i].size;
        *p = '\n';
        p++;
    }
    return buf;
}

void editorOpen (char *filename) {
    free(E.filename);
    E.filename = strdup(filename);

    editorSelectSyntaxHighlight();

    FILE *fp = fopen(filename, "r");
    if (!fp) kill("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorInsertRow(E.numrows, line, linelen);
    }
    free(line);
    fclose(fp);
    E.dirty = 0;
}

void editorSave () {
    if (E.filename == NULL) {
        E.filename = editorPrompt("Save as: %s (ESC: Cancel)", NULL);
        if (E.filename == NULL) {
            editorSetStatusMessage("Save aborted");
            return;
        }
        editorSelectSyntaxHighlight();
    }

    int len;
    char *buf = editorRowsToString(&len);

    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        if (ftruncate(fd, len) != -1)
            if (write(fd, buf, len) == len) {
                close(fd);
                editorSetStatusMessage("%d bytes written to disk", len);
                free(buf);
                E.dirty = 0;
                return;
            }
        close(fd);
    }
    free(buf);
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}
//}}}
// -- Search -- {{{
void editorFindCallback (char *query, int key) {
    static int last_match = -1;
    static int direction = 1;

    static int saved_hl_line;
    static char *saved_hl = NULL;

    if (saved_hl) {
        memcpy(E.row[saved_hl_line].hl, saved_hl, E.row[saved_hl_line].rsize);
        free(saved_hl);
        saved_hl = NULL;
    }

    if (key == '\r' || key == '\x1b') {
        last_match = -1;
        direction = 1;
        return;
    } else if (key == 'n' || key == ARROW_DOWN)
        direction = 1;
    else if (key == 'N' || key == ARROW_UP)
        direction = -1;
    else {
        last_match = -1;
        direction = 1;
    }

    if (last_match == -1) direction = 1;
    int current = last_match;
    int i;
    for (i = 0; i < E.numrows; i++) {
        current += direction;
        if (current == -1) current = E.numrows - 1;
        else if (current == E.numrows) current = 0;

        erow *row = &E.row[current];
        char *match = strstr(row->render, query);
        if (match) {
            last_match = current;
            E.cy = current;
            E.cx = editorRowRxToCx(row, match - row->render);
            E.rowoff = E.numrows;

            saved_hl_line = current;
            saved_hl = malloc(row->rsize);
            memcpy(saved_hl, row->hl, row->rsize);
            memset(&row->hl[match - row->render], HL_MATCH, strlen(query));
            break;
        }
    }
}

void editorFind () {
    int saved_cx = E.cx;
    int saved_cy = E.cy;
    int saved_coloff = E.coloff;
    int saved_rowoff = E.rowoff;
    char *query = editorPrompt("Search: %s (ESC: Cancel | n/N: Move)", editorFindCallback);

    if (query)
        free(query);
    else {
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.coloff = saved_coloff;
        E.rowoff = saved_rowoff;
    }
}
//}}}
// -- Input -- {{{
char *editorPrompt (char *prompt, void (*callback)(char *, int)) {
    size_t bufsize = 128;
    char *buf = malloc(bufsize);
    size_t buflen = 0;
    buf[0] = '\0';

    while (1) {
        editorSetStatusMessage(prompt, buf);
        editorRefreshScreen();

        int c = editorReadKey();
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
            if (buflen != 0) buf[--buflen] = '\0';
        } else if (c == '\x1b') {
            editorSetStatusMessage("");
            if (callback) callback(buf, c);
            free(buf);
            return NULL;
        } else if (c == '\r') {
            if (buflen != 0) {
                editorSetStatusMessage("");
                if (callback) callback(buf, c);
                return buf;
            }
        } else if (!iscntrl(c) && c < 128) {
            if (buflen == bufsize - 1) {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }
        if (callback) callback(buf, c);
    }
}

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
        case UP: case ARROW_UP: if (E.cy != 0)
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
    static int quit_times = QUIT_TIMES;

    int c = editorReadKey();

    switch (c) {
        case '\r':
            editorInsertNewline();
            break;

        case CTRL_KEY('q'):
            if (E.dirty && quit_times > 0) {
                editorSetStatusMessage("WARNING!! File has unsaved changes. "
                        "Press C-q %d more times to quit", quit_times);
                quit_times--;
                return;
            }
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;

        case BACKSPACE: case CTRL_KEY('h'):
        case DEL_KEY:
            if (c == DEL_KEY) editorMoveCursor(RIGHT);
            editorDelChar();
            break;

        case CTRL_KEY('s'):
            editorSave();
            break;

        case CTRL_KEY('l'):
        case '\x1b':
            /* TODO */
            break;

        case CTRL_KEY('f'):
            editorFind();
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

        default:
            editorInsertChar(c);
            break;
    }
    quit_times = QUIT_TIMES;
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

            char *c = &E.row[filerow].render[E.coloff];
            unsigned char *hl = &E.row[filerow].hl[E.coloff];
            int cur_col = -1;
            int i;
            for (i = 0; i < len; i++)
                if (iscntrl(c[i])) {
                    char sym = (c[i] <= 26) ? '@' + c[i] : '?';
                    abAppend(ab, "\x1b[7m", 4);
                    abAppend(ab, &sym, 1);
                    abAppend(ab, "\x1b[m", 3);
                    if (cur_col != -1) {
                        char buf[16];
                        int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", cur_col);
                        abAppend(ab, buf, clen);
                    }
                } else if (hl[i] == HL_NORMAL) {
                    if (cur_col != -1) {
                        abAppend(ab, "\x1b[39m", 5);
                        cur_col = -1;
                    }
                    abAppend(ab, &c[i], 1);
                } else {
                    int color = editorSyntaxToColor(hl[i]);
                    if (color != cur_col) {
                        cur_col = color;
                        char buf[16];
                        int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
                        abAppend(ab, buf, clen);
                    }
                    abAppend(ab, &c[i], 1);
                }
            abAppend(ab, "\x1b[39m", 5);
        }
        abAppend(ab, "\x1b[K", 3);
        /* if (y < E.screenrows - 1) */
        abAppend(ab, "\r\n", 2);
    }
}

void editorRefreshScreen () {
    editorScroll();
    struct abuf ab = ABUF_INIT;
    abAppend(&ab, "\x1b[?25l", 6); // Hide Cursor
    abAppend(&ab, "\x1b[H", 3); // Reposition Cursor

    editorDrawRows(&ab);
    editorDrawStatusBar(&ab);
    editorDrawMessageBar(&ab);

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
    E.filename = NULL;
    E.stsmsg[0] = '\0';
    E.stsmsg_time = 0;
    E.dirty = 0;
    E.syntax = NULL;

    if (getWindowSize(&E.screenrows, &E.screencols) == -1) kill("getWindowSize");
    E.screenrows -= 2;
}

int main (int argc, char *argv[]) {
    enterRawMode();
    initEditor();
    if (argc >= 2) editorOpen(argv[1]);

    editorSetStatusMessage("HELP: C-q: Quit | C-s: Save | C-f: Find");

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
} //}}}
/* <| ------------------------------------------------- |> */
