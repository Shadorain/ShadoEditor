/* ------------------------------- c_search.c ------------------------------- */
#include "shado.h"

void search_callback (char *query, int key) {
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
            E.curs.cy = current;
            E.curs.cx = row_rx_to_cx(row, match - row->render);
            E.curs.rowoff = E.numrows;

            saved_hl_line = current;
            saved_hl = malloc(row->rsize);
            memcpy(saved_hl, row->hl, row->rsize);
            memset(&row->hl[match - row->render], HL_MATCH, strlen(query));
            break;
        }
    }
}

void search () {
    int saved_cx = E.curs.cx;
    int saved_cy = E.curs.cy;
    int saved_coloff = E.curs.coloff;
    int saved_rowoff = E.curs.rowoff;
    char *query = prompt_line("Search: %s (ESC: Cancel | n/N: Move)", search_callback);

    if (query)
        free(query);
    else {
        E.curs.cx = saved_cx;
        E.curs.cy = saved_cy;
        E.curs.coloff = saved_coloff;
        E.curs.rowoff = saved_rowoff;
    }
}
/* -------------------------------------------------------------------------- */
