/* -------------------------------- s_rows.c -------------------------------- */
#include "shado.h"

int row_cx_to_rx (erow *row, int cx) {
    int rx = 0;
    int i;
    for (i = 0; i < cx; i++) { 
         if (row->chars[i] == '\t')
             rx += (TAB_STOP - 1) - (rx % TAB_STOP);
         rx++;
    }
    return rx;
}

int row_rx_to_cx (erow *row, int rx) {
    int cur_rx = 0;
    int cx;
    for (cx = 0; cx < row->size; cx++) { 
        if (row->chars[cx] == '\t')
            cur_rx += (TAB_STOP - 1) - (cur_rx % TAB_STOP);
        cur_rx++;
        if (cur_rx > rx) return cx;
    }
    return cx;
}

void update_row (erow *row) {
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

    update_syntax(row);
}

void insert_row (int at, char *s, size_t len) {
    if (at < 0 || at  > E.numrows) return;

    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
    memmove(&E.row[at+1], &E.row[at], sizeof(erow) * (E.numrows - at));
    for (int j = at + 1; j <= E.numrows; j++) E.row[j].idx++;

    E.row[at].idx = at;
    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';

    E.row[at].rsize = 0;
    E.row[at].render = NULL;
    E.row[at].hl = NULL;
    E.row[at].hl_open_comment = 0;
    update_row(&E.row[at]);
    
    E.numrows++;
    E.dirty++;
}

void free_row (erow *row) {
    free(row->render);
    free(row->chars);
    free(row->hl);
}

void delete_row (int at) {
    if (at < 0 || at >= E.numrows) return;
    free_row(&E.row[at]);
    memmove(&E.row[at], &E.row[at+1], sizeof(erow) * (E.numrows - at - 1));
    for (int j = at; j < E.numrows - 1; j++) E.row[j].idx--;
    E.numrows--;
    E.dirty++;
}

void insert_char_row (erow *row, int at, int c) {
    if (at < 0 || at > row->size) at = row->size;
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    update_row(row);
    E.dirty++;
}

void delete_char_row(erow *row, int at) {
    if (at < 0 || at >= row->size) return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    update_row(row);
    E.dirty++;
}

void append_string_row (erow *row, char *s, size_t len) {
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    update_row(row);
    E.dirty++;
}
/* -------------------------------------------------------------------------- */
