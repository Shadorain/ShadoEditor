/* ------------------------------- s_output.c ------------------------------- */
#include "shado.h"

void scroll () {
    E.rx = 0;
    if (E.cy < E.numrows)
        E.rx = row_cx_to_rx(&E.row[E.cy], E.cx);

    if (E.cy < E.rowoff)
        E.rowoff = E.cy;
    if (E.cy >= E.rowoff + E.screenrows)
        E.rowoff = E.cy - E.screenrows + 1;
    if (E.rx < E.coloff)
        E.coloff = E.rx;
    if (E.rx >= E.coloff + E.screencols)
        E.coloff = E.rx - E.screencols + 1;
}

void draw_rows (struct abuf *ab) {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        int filerow = y + E.rowoff;
        if (filerow >= E.numrows) {
            if (E.numrows == 0 && y == E.screenrows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                        "Shado editor -- version %s", SHADO_VERSION);
                if (welcomelen > E.screencols) welcomelen = E.screencols;
                int padding = (E.screencols - welcomelen) / 2;
                if (padding) {
                    ab_append(ab, "~", 1);
                    padding--;
                }
                while (padding--) ab_append(ab, " ", 1);
                ab_append(ab, welcome, welcomelen);
            } else
                ab_append(ab, "~", 1);
        } else {
            int len = E.row[filerow].rsize - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols) len = E.screencols;

            char *c = &E.row[filerow].render[E.coloff];
            /* unsigned char *hl = &E.row[filerow].hl[E.coloff]; */
            int cur_col = -1;
            int i;
            for (i = 0; i < len; i++)
                if (iscntrl(c[i])) {
                    char sym = (c[i] <= 26) ? '@' + c[i] : '?';
                    ab_append(ab, "\x1b[7m", 4);
                    ab_append(ab, &sym, 1);
                    ab_append(ab, "\x1b[m", 3);
                    if (cur_col != -1) {
                        char buf[16];
                        int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", cur_col);
                        ab_append(ab, buf, clen);
                    }
                /* } */ 
                /* else if (hl[i] == HL_NORMAL) { */
                /*     if (cur_col != -1) { */
                /*         ab_append(ab, "\x1b[39m", 5); */
                /*         cur_col = -1; */
                /*     } */
                /*     ab_append(ab, &c[i], 1); */
                } else {
                    /* int color = syntax_to_color(hl[i]); */
                    /* if (color != cur_col) { */
                    /*     cur_col = color; */
                    /*     char buf[16]; */
                    /*     int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color); */
                    /*     ab_append(ab, buf, clen); */
                    /* } */
                    /* ab_append(ab, &c[i], 1); */
                }
            ab_append(ab, "\x1b[39m", 5);
        }
        ab_append(ab, "\x1b[K", 3);
        /* if (y < E.screenrows - 1) */
        ab_append(ab, "\r\n", 2);
    }
}

void refresh_screen () {
    scroll();
    struct abuf ab = ABUF_INIT;
    ab_append(&ab, "\x1b[?25l", 6); /* Hide Cursor */
    ab_append(&ab, "\x1b[H", 3); /* Reposition Cursor */

    draw_rows(&ab);
    draw_sts_bar(&ab);
    draw_msg_bar(&ab);

    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1,
                                              (E.rx - E.coloff) + 1);
    ab_append(&ab, buf, strlen(buf));
    ab_append(&ab, "\x1b[?25h", 6); /* Unhide Cursor */

    write(STDOUT_FILENO, ab.b, ab.len);
    ab_free(&ab);
}

void set_cursor_type () {
    if(E.mode == 1) write(STDOUT_FILENO, "\x1b[6 q", 5);
    else if(E.mode == 10) write(STDOUT_FILENO, "\x1b[4 q", 5);
    else write(STDOUT_FILENO, "\x1b[2 q", 5);
}
/* -------------------------------------------------------------------------- */
