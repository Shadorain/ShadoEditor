/* -------------------------------- s_input.c ------------------------------- */
#include "shado.h"
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
/* -- Move Cursor -- {{{ */
void editorMoveCursor (int key) {
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

    switch (key) {
        case LEFT:
            if (E.cx != 0)
                E.cx--;
            /* else if (E.cy > 0) { */
            /*     E.cy--; */
            /*     E.cx = E.row[E.cy].size; */
            /* } */
            break;
        case DOWN:
            if (E.cy < E.numrows)
                E.cy++;
            break;
        case UP:
            if (E.cy != 0)
                E.cy--;
            break;
        case RIGHT:
            if (row && E.cx < row->size)
                E.cx++;
            /* else if (row && E.cx == row->size) { */
            /*     E.cy++; */
            /*     E.cx = 0; */
            /* } */
            break;

        case HOME_KEY:
            E.cx = 0;
            break;
        case END_KEY:
            if (E.cy < E.numrows)
                E.cx = E.row[E.cy].size;
            break;

        case PAGE_UP:
            E.cy = E.rowoff;
            int times = E.screenrows;
            while (times--)
                editorMoveCursor(UP);
            break;
        case PAGE_DOWN:
            E.cy = E.rowoff + E.screenrows - 1;
            if (E.cy > E.numrows) E.cy = E.numrows;
            times = E.screenrows;
            while (times--)
                editorMoveCursor(DOWN);
            break;
    }

    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen)
        E.cx = rowlen;
}
/*}}}*/
/* -------------------------------------------------------------------------- */
