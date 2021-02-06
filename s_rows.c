/* -------------------------------- s_rows.c -------------------------------- */
#include "shado.h"

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

int editorRowRxToCx (erow *row, int rx) {
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

    editorUpdateSyntax(row);
}

void editorInsertRow (int at, char *s, size_t len) {
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
    editorUpdateRow(&E.row[at]);
    
    E.numrows++;
    E.dirty++;
}

void editorFreeRow (erow *row) {
    free(row->render);
    free(row->chars);
    free(row->hl);
}

void editorDelRow (int at) {
    if (at < 0 || at >= E.numrows) return;
    editorFreeRow(&E.row[at]);
    memmove(&E.row[at], &E.row[at+1], sizeof(erow) * (E.numrows - at - 1));
    for (int j = at; j < E.numrows - 1; j++) E.row[j].idx--;
    E.numrows--;
    E.dirty++;
}

void editorRowInsertChar (erow *row, int at, int c) {
    if (at < 0 || at > row->size) at = row->size;
    row->chars = realloc(row->chars, row->size + 2);
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    row->size++;
    row->chars[at] = c;
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowDelChar(erow *row, int at) {
    if (at < 0 || at >= row->size) return;
    memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
    row->size--;
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowAppendString (erow *row, char *s, size_t len) {
    row->chars = realloc(row->chars, row->size + len + 1);
    memcpy(&row->chars[row->size], s, len);
    row->size += len;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
    E.dirty++;
}
/* -------------------------------------------------------------------------- */
