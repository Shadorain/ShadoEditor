/* --------------------------------- s_bar.c -------------------------------- */
#include "shado.h"

void draw_sts_bar (struct abuf *ab) {
    ab_append(ab, "\x1b[7m", 4);
    char status[80], rstatus[80];
    /* int len = snprintf(status, sizeof(status), "%.20s - %d lines", */
    /*         E.filename ? E.filename : "[No Name]", E.numrows); */
    int len = snprintf(status, sizeof(status), "%.20s %s",
            E.filename ? E.filename : "[No Name]",
            E.dirty ? "(modified)" : "");
    int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d:%d",
            "no ft", E.cy + 1, E.numrows);
            /* E.syntax ? E.syntax->filetype : "no ft", E.cy + 1, E.numrows); */
    if (len > E.screencols) len = E.screencols;
    ab_append(ab, status, len);
    while (len < E.screencols) {
        if (E.screencols - len == rlen) {
            ab_append(ab, rstatus, rlen);
            break;
        } else {
            ab_append(ab, " ", 1);
            len++;
        }
    }
    ab_append(ab, "\x1b[m", 3);
    ab_append(ab, "\r\n", 2);
}

void draw_msg_bar (struct abuf *ab) {
    ab_append(ab, "\x1b[K", 3);
    int msglen = strlen(E.stsmsg);
    if (msglen > E.screencols) msglen = E.screencols;
    if (msglen && time(NULL) - E.stsmsg_time < 5)
        ab_append(ab, E.stsmsg, msglen);
}

void set_sts_msg (const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.stsmsg, sizeof(E.stsmsg), fmt, ap);
    va_end(ap);
    E.stsmsg_time = time(NULL);
}
/* -------------------------------------------------------------------------- */
