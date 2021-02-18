/* -------------------------------- s_modes.c ------------------------------- */
#include "shado.h"
/* -- Normal -- {{{ */
/* typedef struct func_args { */
/*     int arg; */
/* } funcarg_t; */
/* typedef void (*nfunc_t)(funcarg_t *args); */

/* --- Prototypes --- {{{ */
void n_append();
void n_eappend();
void n_nldown();
void n_nlup();
void n_cursdel();
void n_pcursdel();
void n_idel();
void n_iedel();
void n_gototop();
void n_gotobottom();
void n_nlup();
void n_ctree();
void n_dtree();
void n_gtree();
void n_ytree();
void n_ztree();
void n_insert();
void n_null();
void n_move_left();
void n_move_down();
void n_move_up();
void n_move_right();
void n_return();
void n_quit();
/* --- Proto: Delete --- {{{ */
void n_dline();
void n_ddown();
void n_dup();
void n_dleft();
void n_dright();
/*  }}} */
/* --- Proto: Change --- {{{ */
void n_cline();
void n_cdown();
void n_cup();
void n_cleft();
void n_cright();
/*  }}} */
/* }}} */
const struct mapping n_map[] = { 
    {'a', n_append},
    {'A', n_eappend},
    {'i', n_insert},
    {'o', n_nldown},
    {'O', n_nlup},
    {'H', n_gototop},
    {'L', n_gotobottom},
    {'s', n_idel},
    {'S', n_iedel},
    {'x', n_cursdel},
    {'X', n_pcursdel},
    {'c', n_ctree},
    {'d', n_dtree},
    {'g', n_gtree},
    {'y', n_ytree},
    {'z', n_ztree},
    {'h', n_move_left},
    {'j', n_move_down},
    {'k', n_move_up},
    {'l', n_move_right},
    {CTRL_KEY('q'), n_quit},
    {'\r', n_return},
};

/* const struct mapping n_cmap[] = { */
/* }; */

void n_append() {
    editorMoveCursor(RIGHT);
    E.mode = INSERT;
    changeCursorShape();
}

void n_eappend() {
    editorMoveCursor(END_KEY);
    E.mode = INSERT;
    changeCursorShape();
}

void n_insert() {
    E.mode = INSERT;
    changeCursorShape();
}

void n_nldown() {
    editorMoveCursor(END_KEY);
    editorInsertNewline();
    E.mode = INSERT;
    changeCursorShape();
}

void n_nlup() {
    editorMoveCursor(UP);
    editorMoveCursor(END_KEY);
    editorInsertNewline();
    E.mode = INSERT;
    changeCursorShape();
}

void n_cursdel() {
    editorMoveCursor(RIGHT);
    editorDelChar();
}

void n_pcursdel() {
    editorDelChar();
}

void n_idel() {
    editorMoveCursor(RIGHT);
    editorDelChar();
    E.mode = INSERT;
    changeCursorShape();
}

void n_iedel() {
    editorDelChar();
    E.mode = INSERT;
    changeCursorShape();
}

void n_gototop() { editorMoveCursor(PAGE_UP); }
void n_gotobottom() { editorMoveCursor(PAGE_DOWN); }

void n_gtree() { return; }
void n_ytree() { return; }
void n_ztree() { return; }

void n_move_left() {
    editorMoveCursor(LEFT);
}

void n_move_down() {
    editorMoveCursor(DOWN);
}

void n_move_up() {
    editorMoveCursor(UP);
}

void n_move_right() {
    editorMoveCursor(RIGHT);
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
    /* TODO: make more like vims */
    editorMoveCursor(DOWN);
}
/* --- Delete --- {{{ */
const struct mapping n_dmap[] = {
    {'d', n_dline},
    {'j', n_ddown},
    {'k', n_dup},
    {'h', n_dleft},
    {'l', n_dright},
};

void n_dtree() { 
    E.mode = MISC;
    changeCursorShape();

    int c = editorReadKey();
    for (int i = 0; i < LEN(n_dmap); ++i)
        if (n_dmap[i].c == c) {
            n_dmap[i].cmd_func();
            break;
        }

    E.mode = NORMAL;
    changeCursorShape();
}

void n_dline() { editorDelRow(E.cy); }
void n_ddown() {
    editorDelRow(E.cy);
    editorDelRow(E.cy);
}
void n_dup() {
    editorDelRow(E.cy);
    editorMoveCursor(UP);
    editorDelRow(E.cy);
}
void n_dleft() {
    editorDelChar();
}
void n_dright() {
    editorMoveCursor(RIGHT);
    editorMoveCursor(RIGHT);
    editorDelChar();
}
/*  }}} */
/* --- Change --- {{{ */
const struct mapping n_cmap[] = {
    {'c', n_cline},
    {'j', n_cdown},
    {'k', n_cup},
    {'h', n_cleft},
    {'l', n_cright},
};

void n_ctree() { 
    E.mode = MISC;
    changeCursorShape();

    int c = editorReadKey();
    for (int i = 0; i < LEN(n_cmap); ++i)
        if (n_cmap[i].c == c) {
            n_cmap[i].cmd_func();
            E.mode = INSERT;
            break;
        } else {
            E.mode = NORMAL;
        }
    changeCursorShape();
}

void n_cline() {
    editorDelRow(E.cy);
    editorMoveCursor(UP);
    editorInsertNewline();
    E.mode = INSERT;
    changeCursorShape();
}
void n_cdown() {
    editorDelRow(E.cy);
    editorDelRow(E.cy);
    E.mode = INSERT;
    changeCursorShape();
}
void n_cup() {
    editorDelRow(E.cy);
    editorMoveCursor(UP);
    editorDelRow(E.cy);
    E.mode = INSERT;
    changeCursorShape();
}
void n_cleft() {
    editorDelChar();
    E.mode = INSERT;
    changeCursorShape();
}
void n_cright() {
    editorMoveCursor(RIGHT);
    editorMoveCursor(RIGHT);
    editorDelChar();
    E.mode = INSERT;
    changeCursorShape();
}
/*  }}} */
/*}}}*/
/* -- Insert -- {{{ */
/* --- Prototypes --- {{{ */
void i_null();
void i_move_left();
void i_move_down();
void i_move_up();
void i_move_right();
void i_backspace();
void i_delet();
void i_escape();
void i_return();
/* }}} */
const struct mapping i_map[] = { 
    {1000, i_move_left},
    {1001, i_move_down},
    {1002, i_move_up},
    {1003, i_move_right},
    {27,   i_escape},
    {127,  i_backspace},
    {'\r', i_return},
};

void i_move_left() {
    editorMoveCursor(LEFT);
    E.print_flag = 0;
}

void i_move_down() {
    editorMoveCursor(DOWN);
    E.print_flag = 0;
}

void i_move_up() {
    editorMoveCursor(UP);
    E.print_flag = 0;
}

void i_move_right() {
    editorMoveCursor(RIGHT);
    E.print_flag = 0;
}

void i_null() {
    E.print_flag = 0;
    return;
}

void i_escape() {
    editorMoveCursor(LEFT);
    E.mode = NORMAL;
    E.print_flag = 0;
    changeCursorShape();
}

void i_backspace() {
    editorDelChar();
    E.print_flag = 0;
}

void i_delete() {
    editorMoveCursor(RIGHT);
    editorDelChar();
    E.print_flag = 0;
}

void i_return() {
    editorInsertNewline();
    E.print_flag = 0;
}
/*}}}*/
/* -- Process Keypress -- {{{ */
void process_keypress () {
    /* static int quit_times = QUIT_TIMES; */

    int c = editorReadKey();
    int mode = E.mode;

    /* Normal Mode */
    /* TODO: Sort structs and search via binary search instead of brute */
    if (mode == NORMAL)
        for (int i = 0; i < LEN(n_map); ++i)
            if (n_map[i].c == c) {
                n_map[i].cmd_func();
                break;
            }
    if (mode == INSERT) {
        for (int j = 0; j < LEN(i_map); ++j)
            if (i_map[j].c == c) {
                i_map[j].cmd_func();
                break;
            }
        if (E.mode == INSERT && E.print_flag == 1) // secondary check just incase
            editorInsertChar(c);
    }

    E.print_flag = 1;
    /* quit_times = QUIT_TIMES; */
}

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
/*     case CTRL_KEY('s'): */
/*         editorSave(); */
/*         break; */
/*     case CTRL_KEY('f'): */
/*         editorFind(); */
/*         break; */
/*}}}*/
/* -------------------------------------------------------------------------- */
