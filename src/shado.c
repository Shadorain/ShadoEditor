/* <| Shado :: a lightweight, modular, and modal editor |> */
#include "shado.h"

struct GlobalState E;
/* struct Stack *undo; */
/* struct Stack *redo; */

struct GlobalState *make_snapshot () {
    /* struct GlobalState *new = malloc(sizeof(struct GlobalState)); */

    /* new->cx = E.curs.cx; */
    /* new->cy = E.curs.cy; */
    /* new->rx = E.curs.rx; */
    /* new->rowoff = E.curs.rowoff; */
    /* new->coloff = E.curs.coloff; */
    /* new->screenrows = E.screenrows; */
    /* new->screencols = E.screencols; */
    /* new->dirty = E.dirty; */
    /* new->numrows = E.numrows; */
    /* new->filename = E.filename; */
    /* new->stsmsg_time = E.stsmsg_time; */
    /* new->syntax = E.syntax; */
    /* new->orig_termios = E.orig_termios; */
    /* new->mode = E.mode; */
    /* new->stsmsg[0] = *E.stsmsg; */

    /* new->row = malloc(sizeof(erow) * (E.numrows)); */
    /* int i; */
    /* for (i = 0; i < new->numrows; i++){ */
    /*   new->row[i].idx = E.row[i].idx; */
    /*   new->row[i].size = E.row[i].size; */
    /*   new->row[i].rsize = E.row[i].rsize; */
    /*   new->row[i].hl_open_comment = E.row[i].hl_open_comment; */

    /*   new->row[i].chars = malloc(E.row[i].size); */
    /*   memcpy(new->row[i].chars, E.row[i].chars, E.row[i].size); */

    /*   new->row[i].render = malloc(E.row[i].rsize); */
    /*   memcpy(new->row[i].render, E.row[i].render, E.row[i].rsize); */

    /*   new->row[i].hl = malloc(E.row[i].rsize); */
    /*   memcpy(new->row[i].hl, E.row[i].hl, E.row[i].rsize); */
    /* } */
    /* return new; */
    /* /1* struct GlobalState *snapshot = &E; *1/ */
    /* /1* return snapshot; *1/ */
    return NULL;
}

#ifdef DEBUG
void print_debug () {
    printf("======= GlobalState Debug Info =======\n\r");
    printf("------- Cursor -------\n\r");
    printf("cx: %d\n\rcy: %d\n\rrx: %d\n\r", E.curs.cx, E.curs.cy, E.curs.rx);
    printf("------- Rows -------\n\r");
    printf("rowoff: %d\n\rcoloff: %d\n\r", E.curs.rowoff, E.curs.coloff);
    printf("screenrows: %d\n\rscreencols: %d\n\r", E.screenrows, E.screencols);
    printf("numrows: %d\n\r", E.numrows);
    /* printf("row (chars): %s\n\r", E.row[E.curs.cy].chars); */
    /* printf("row (render): %s\n\r", E.row[E.curs.cy].render); */
    printf("------- Misc -------\n\r");
    printf("dirty: %d\n\r", E.dirty);
    printf("filename: %s\n\r", E.filename);
    printf("stsmsg: %s\n\r", E.stsmsg);
    /* printf("------- Syntax -------\n\r"); */
    /* printf("syntax ft: %s\n\r", E.syntax->filetype); */
    printf("------- Extra -------\n\r");
    printf("mode: %d\n\rprint flag: %d\n\r", E.mode, E.print_flag);
    /* printf("------- CopyRegister -------\n\r"); */
    /* printf("cpyhead: %s\n\r", E.cpyhead->line); */
    /* printf("cpycurr: %s\n\r", E.cpycurr->line); */
    printf("=======#----------------------#=======\n\r");
    /* printf("======= Undo & Redo =======\n\r"); */
    /* printf("------- Undo Stack -------\n\r"); */
    /* printf("undo peek: %d\n\r", peek(undo)->mode); */
    /* printf("redo peek: %d\n\r", peek(redo)->mode); */
    /* printf("=======#-----------#=======\n\r"); */
}
#endif

void init () {
    E.curs.cx = 0;
    E.curs.cy = 0;
    E.curs.rx = 0;
    E.curs.rowoff = 0;
    E.curs.coloff = 0;
    E.numrows = 0;
    E.filename = NULL;
    E.stsmsg[0] = '\0';
    E.stsmsg_time = 0;
    E.dirty = 0;
    /* E.syntax = NULL; */

    E.row = NULL;
    E.rope_head = rope_new(); /* moving to this */

    E.mode = NORMAL;
    E.print_flag = 1;
    /* E.cpyhead = NULL; */

    if (get_win_size(&E.screenrows, &E.screencols) == -1) kill("getWindowSize");
    E.screenrows -= 2;

    /* undo = NULL; */
    /* redo = NULL; */
}

int main (int argc, char *argv[]) {
    enable_raw();
    init();
    if (argc >= 2) open_file(argv[1]);

    /* set_sts_msg("HELP: C-q: Quit | C-s: Save | C-f: Find"); */

    /* while (1) { */
    /*     refresh_screen(); */
    /*     process_keypress(); */
    /* } */
    quit();
    return 0;
}
/* <| ------------------------------------------------- |> */
