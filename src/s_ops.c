/* --------------------------------- s_ops.c -------------------------------- */
#include "shado.h"
#include <ctype.h>
#include "../lib/rope.h"

void insert_char (int c) {
    if (E.cy == E.numrows)
        insert_row(E.numrows, "", 0);
    /* if (c == '\n') insert_char_row(&E.row[E.cy], E.cx, '\n'); */
    if (c == '\n') return;
    else if (c == '\r') {
        erow *row = &E.row[E.cy];
        insert_row(E.cy+1, &row->chars[E.cx], row->size - E.cx);
        row = &E.row[E.cy];
        row->size = E.cx;
        row->chars[row->size] = '\0';
        update_row(row);
        E.cy++;
    } else 
        insert_char_row(&E.row[E.cy], E.cx, c);
    E.cx++;
}

void delete_char () {
    if (E.cy == E.numrows) return;
    if (E.cx == 0 && E.cy == 0) return;

    erow *row = &E.row[E.cy];
    if (E.cx > 0) {
        delete_char_row(row, E.cx - 1);
        E.cx--;
    } else {
        E.cx = E.row[E.cy-1].size;
        append_string_row(&E.row[E.cy-1], row->chars, row->size);
        delete_row(E.cy);
        E.cy--;
    }
}

/* char get_char () { */
/*     /1* if (E.cy == E.numrows) return ; *1/ */
/*     /1* if (E.cx == 0 && E.cy == 0) return; *1/ */

/*     erow *row = &E.row[E.cy]; */
/*     if (E.cx > 0) { */
/*         delete_char_row(row, E.cx - 1); */
/*         E.cx--; */
/*     } else { */
/*         E.cx = E.row[E.cy-1].size; */
/*         append_string_row(&E.row[E.cy-1], row->chars, row->size); */
/*         delete_row(E.cy); */
/*         E.cy--; */
/*     } */
/* } */

void insert_nl () {
    if (E.cx == 0)
        insert_row(E.cy, "", 0);
    else {
        erow *row = &E.row[E.cy];
        insert_row(E.cy+1, &row->chars[E.cx], row->size - E.cx);
        row = &E.row[E.cy];
        row->size = E.cx;
        row->chars[row->size] = '\0';
        update_row(row);
    }
    E.cy++;
    E.cx = 0;
}

int get_char_type (int c) {
    if (isspace(c)) return CHAR_WHITESPACE;
    if (is_separator(c)) return CHAR_SYM;
    if (c == '\n') return CHAR_NL;
    rope *r=rope_new();
    _rope_print(r);
    return CHAR_AZ09;
}
/* -------------------------------------------------------------------------- */
