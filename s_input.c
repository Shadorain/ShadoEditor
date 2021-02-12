/* -------------------------------- s_input.c ------------------------------- */
#include "shado.h"
/* typedef struct func_args { */
/*     int arg; */
/* } funcarg_t; */
/* typedef void (*nfunc_t)(funcarg_t *args); */

void n_append();
void n_null();
void n_return();
void n_quit();

typedef void (*handle)(void);

const struct mapping {
    char c;
    handle cmd_func;
} n_map[] = { 
        {'a', n_append},
        {'q', n_quit},
        {'\r', n_return},
};

void n_append() {
    editorMoveCursor(RIGHT);
    E.mode = INSERT;
}

void n_null() {
    return;
}

void n_quit() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    return;
}

void n_return() {
    editorInsertNewline();
}
// -- Prompt -- {{{
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
//}}}
// -- Move Cursor -- {{{
void editorMoveCursor (int key) {
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

    switch (key) {
        case LEFT: //case ARROW_LEFT:
            if (E.cx != 0)
                E.cx--;
            else if (E.cy > 0) {
                E.cy--;
                E.cx = E.row[E.cy].size;
            }
            break;
        case DOWN: //case ARROW_DOWN:
            if (E.cy < E.numrows)
                E.cy++;
            break;
        case UP: //case ARROW_UP: if (E.cy != 0)
                E.cy--;
            break;
        case RIGHT: //case ARROW_RIGHT:
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
//}}}
// -- Process Keypress -- {{{
void editorProcessKeypress () {
    static int quit_times = QUIT_TIMES;

    char c = editorReadKey();
    int mode = E.mode;

    // Normal Mode
    if (mode == 0)
        for (int i = 0; i < LEN(n_map); ++i)
            if (n_map[i].c == c) n_map[i].cmd_func();

    quit_times = QUIT_TIMES;
}

        /* switch (c) { */
        /*     case '\r': */
        /*         editorInsertNewline(); */
        /*         break; */

        /*     case CTRL_KEY('q'): */
        /*         if (E.dirty && quit_times > 0) { */
        /*             editorSetStatusMessage("WARNING!! File has unsaved changes. " */
        /*                     "Press C-q %d more times to quit", quit_times); */
        /*             quit_times--; */
        /*             return; */
        /*         } */
        /*         write(STDOUT_FILENO, "\x1b[2J", 4); */
        /*         write(STDOUT_FILENO, "\x1b[H", 3); */
        /*         exit(0); */
        /*         break; */

        /*     case BACKSPACE: case CTRL_KEY('h'): //case DEL_KEY: */
        /*         if (c == DEL_KEY) editorMoveCursor(RIGHT); */
        /*         editorDelChar(); */
        /*         break; */

        /*     case CTRL_KEY('s'): */
        /*         editorSave(); */
        /*         break; */

        /*     case CTRL_KEY('l'): */
        /*     case '\x1b': */
        /*         /1* TODO *1/ */
        /*         break; */

        /*     case CTRL_KEY('f'): */
        /*         editorFind(); */
        /*         break; */

        /*     case HOME_KEY: */
        /*         E.cx = 0; */
        /*         break; */
        /*     case END_KEY: */
        /*         if (E.cy < E.numrows) */
        /*             E.cx = E.row[E.cy].size; */
        /*         break; */

        /*     case PAGE_UP: case PAGE_DOWN: */
        /*         { */
        /*             if (c == PAGE_UP) */
        /*                 E.cy = E.rowoff; */
        /*             else if (c == PAGE_DOWN) { */
        /*                 E.cy = E.rowoff + E.screenrows - 1; */
        /*                 if (E.cy > E.numrows) E.cy = E.numrows; */
        /*             } */

        /*             int times = E.screenrows; */
        /*             while (times--) */
        /*                 editorMoveCursor(c == PAGE_UP ? UP : DOWN); */
        /*         } */
        /*         break; */

        /*     case LEFT: case DOWN: case UP: case RIGHT: */
        /*     case ARROW_LEFT: case ARROW_DOWN: case ARROW_UP: case ARROW_RIGHT: */
        /*         editorMoveCursor(c); */
        /*         break; */

        /*     default: */
        /*         editorInsertChar(c); */
        /*         break; */
        /* } */
//}}}
/* -------------------------------------------------------------------------- */
