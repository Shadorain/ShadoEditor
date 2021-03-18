/* --------------------------------- s_ops.c -------------------------------- */
#include "shado.h"
#include <ctype.h>
#include "../lib/rope.h"

void insert_char (int c) {
    if (E.curs.cy == E.numrows)
        insert_row(E.numrows, "", 0);
    /* if (c == '\n') insert_char_row(&E.row[E.curs.cy], E.curs.cx, '\n'); */
    if (c == '\n') return;
    else if (c == '\r') {
        erow *row = &E.row[E.curs.cy];
        insert_row(E.curs.cy+1, &row->chars[E.curs.cx], row->size - E.curs.cx);
        row = &E.row[E.curs.cy];
        row->size = E.curs.cx;
        row->chars[row->size] = '\0';
        update_row(row);
        E.curs.cy++;
    } else 
        insert_char_row(&E.row[E.curs.cy], E.curs.cx, c);
    E.curs.cx++;
}

void delete_char () {
    if (E.curs.cy == E.numrows) return;
    if (E.curs.cx == 0 && E.curs.cy == 0) return;

    erow *row = &E.row[E.curs.cy];
    if (E.curs.cx > 0) {
        delete_char_row(row, E.curs.cx - 1);
        E.curs.cx--;
    } else {
        E.curs.cx = E.row[E.curs.cy-1].size;
        append_string_row(&E.row[E.curs.cy-1], row->chars, row->size);
        delete_row(E.curs.cy);
        E.curs.cy--;
    }
}

/* char get_char () { */
/*     /1* if (E.curs.cy == E.numrows) return ; *1/ */
/*     /1* if (E.curs.cx == 0 && E.curs.cy == 0) return; *1/ */

/*     erow *row = &E.row[E.curs.cy]; */
/*     if (E.curs.cx > 0) { */
/*         delete_char_row(row, E.curs.cx - 1); */
/*         E.curs.cx--; */
/*     } else { */
/*         E.curs.cx = E.row[E.curs.cy-1].size; */
/*         append_string_row(&E.row[E.curs.cy-1], row->chars, row->size); */
/*         delete_row(E.curs.cy); */
/*         E.curs.cy--; */
/*     } */
/* } */

void insert_nl () {
    if (E.curs.cx == 0)
        insert_row(E.curs.cy, "", 0);
    else {
        erow *row = &E.row[E.curs.cy];
        insert_row(E.curs.cy+1, &row->chars[E.curs.cx], row->size - E.curs.cx);
        row = &E.row[E.curs.cy];
        row->size = E.curs.cx;
        row->chars[row->size] = '\0';
        update_row(row);
    }
    E.curs.cy++;
    E.curs.cx = 0;
}

int get_char_type (int c) {
    if (isspace(c)) return CHAR_WHITESPACE;
    if (is_separator(c)) return CHAR_SYM;
    if (c == '\n') return CHAR_NL;
    rope *r=rope_new();
    _rope_print(r);
    return CHAR_AZ09;
}

int is_separator (int c) {
    return isspace(c) || c == '\0' || strchr(",.()+-/*=~%><>[];", c) != NULL;
}
/* -------------------------------------------------------------------------- */
