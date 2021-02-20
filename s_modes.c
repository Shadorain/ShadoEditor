/* -------------------------------- s_modes.c ------------------------------- */
#include "shado.h"
#include <string.h>
/* -- Normal -- {{{ */
/* typedef struct func_args { */
/*     int arg; */
/* } funcarg_t; */
/* typedef void (*nfunc_t)(funcarg_t *args); */

/* --- Prototypes --- {{{ */
void n_append();
void n_eappend();
void n_escape();
void n_exmode();
void n_nldown();
void n_nlup();
void n_cursdel();
void n_pcursdel();
void n_gototop();
void n_gotobottom();
void n_idel();
void n_iedel();
void n_join();
void n_nlup();
void n_ctree();
void n_dtree();
void n_gtree();
void n_ytree();
void n_ztree();
void n_finsert();
void n_insert();
void n_null();
void n_fprint();
void n_bprint();
void n_begin();
void n_end();
void n_search();
/* void n_incsearch(); */
/* void n_decsearch(); */
void n_move_left();
void n_move_down();
void n_move_up();
void n_move_right();
void n_return();
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
/* --- Proto: Yank --- {{{ */
void n_yline();
void n_ydown();
void n_yup();
void n_yleft();
void n_yright();
/*  }}} */
/* }}} */
const struct mapping n_map[] = { 
    {'\r', n_return}, /* 13 */
    /* {CTRL_KEY('q'), n_quit}, /1* 17 *1/ */
    {27,  n_escape}, /* 27 */
    {'$', n_end}, /* 36 */
    {'/', n_search}, /* 47 */
    {':', n_exmode}, /* 58 */
    {'A', n_eappend}, /* 65 */
    {'H', n_gototop}, /* 72 */
    {'I', n_finsert}, /* 73 */
    {'J', n_join}, /* 74 */
    {'L', n_gotobottom}, /* 76 */
    /* {'N', n_decsearch}, /1* 78 *1/ */
    {'O', n_nlup}, /* 79 */
    {'P', n_bprint}, /* 80 */
    {'S', n_iedel}, /* 83 */
    {'X', n_pcursdel}, /* 88 */
    {'^', n_begin}, /* 94 */
    {'a', n_append}, /* 97 */
    {'c', n_ctree}, /* 99 */
    {'d', n_dtree}, /* 100 */
    {'g', n_gtree}, /* 103 */
    {'h', n_move_left}, /* 104 */
    {'i', n_insert}, /* 105 */
    {'j', n_move_down}, /* 106 */
    {'k', n_move_up}, /* 107 */
    {'l', n_move_right}, /* 108 */
    /* {'n', n_incsearch}, /1* 110 *1/ */
    {'o', n_nldown}, /* 111 */
    {'p', n_fprint}, /* 112 */
    {'s', n_idel}, /* 115 */
    {'x', n_cursdel}, /* 120 */
    {'y', n_ytree}, /* 121 */
    {'z', n_ztree}, /* 122 */
};
/* --- Functions --- {{{ */
void n_append() {
    move_cursor(RIGHT);
    E.mode = INSERT;
    set_cursor_type();
}

void n_eappend() {
    move_cursor(END_KEY);
    E.mode = INSERT;
    set_cursor_type();
}

void n_begin () {
    move_cursor(HOME_KEY);
}
void n_end () {
    move_cursor(END_KEY);
}

void n_escape() {
    E.mode = NORMAL;
    return;
}

void n_search() {
    search();
}

/* void n_incsearch() { */
/*     /1* search_callback(char *query, int key) *1/ */
/* } */

/* void n_decsearch() { */
/*     /1* search_callback(char *query, int key) *1/ */
/* } */

void n_insert() {
    E.mode = INSERT;
    set_cursor_type();
}

void n_finsert() {
    /* TODO: go to first non whitespace char */
    E.cx = 0;
    E.mode = INSERT;
    set_cursor_type();
}

void n_nldown() {
    move_cursor(END_KEY);
    insert_nl();
    E.mode = INSERT;
    set_cursor_type();
}

void n_nlup() {
    move_cursor(UP);
    move_cursor(END_KEY);
    insert_nl();
    E.mode = INSERT;
    set_cursor_type();
}

void n_cursdel() {
    move_cursor(RIGHT);
    delete_char();
}

void n_pcursdel() {
    delete_char();
}

void n_fprint() {
    /* TODO */
    /* E.row[E.cy].chars */
}

void n_bprint() {
    /* TODO */
}

void n_join() {
  if (E.cy == E.numrows - 1)
    return;
  erow *row = &E.row[E.cy];
  erow *rowBelow = &E.row[E.cy + 1];
  append_string_row(row, " ", 1);
  append_string_row(row, rowBelow->chars, rowBelow->size);
  delete_row(E.cy + 1);
}

void n_idel() {
    move_cursor(RIGHT);
    delete_char();
    E.mode = INSERT;
    set_cursor_type();
}

void n_iedel() {
    delete_char();
    E.mode = INSERT;
    set_cursor_type();
}

void n_gototop() { move_cursor(PAGE_UP); }
void n_gotobottom() { move_cursor(PAGE_DOWN); }

void n_gtree() { return; }
void n_ztree() { return; }

void n_move_left() {
    move_cursor(LEFT);
}

void n_move_down() {
    move_cursor(DOWN);
}

void n_move_up() {
    move_cursor(UP);
}

void n_move_right() {
    move_cursor(RIGHT);
}

void n_null() {
    return;
}

void n_return() {
    /* TODO: make more like vims */
    move_cursor(DOWN);
}
/*}}}*/
/* --- Change --- {{{ */
const struct mapping n_cmap[] = {
    {'c', n_cline},  /* 99 */
    {'h', n_cleft},  /* 104 */
    {'j', n_cdown},  /* 106 */
    {'k', n_cup},    /* 107 */
    {'l', n_cright}, /* 108 */
};

void n_ctree() { 
    E.mode = MISC;
    set_cursor_type();

    int c = read_keypress();
    for (int i = 0; i < LEN(n_cmap); ++i)
        if (n_cmap[i].c == c) {
            n_cmap[i].cmd_func();
            E.mode = INSERT;
            break;
        } else {
            E.mode = NORMAL;
        }
    set_cursor_type();
}

void n_cline() {
    delete_row(E.cy);
    move_cursor(UP);
    insert_nl();
    E.mode = INSERT;
    set_cursor_type();
}
void n_cdown() {
    delete_row(E.cy);
    delete_row(E.cy);
    E.mode = INSERT;
    set_cursor_type();
}
void n_cup() {
    delete_row(E.cy);
    move_cursor(UP);
    delete_row(E.cy);
    E.mode = INSERT;
    set_cursor_type();
}
void n_cleft() {
    delete_char();
    E.mode = INSERT;
    set_cursor_type();
}
void n_cright() {
    move_cursor(RIGHT);
    move_cursor(RIGHT);
    delete_char();
    E.mode = INSERT;
    set_cursor_type();
}
/*  }}} */
/* --- Delete --- {{{ */
const struct mapping n_dmap[] = {
    {'d', n_dline}, /* 100 */
    {'h', n_dleft}, /* 104 */
    {'j', n_ddown}, /* 106 */
    {'k', n_dup},   /* 107 */
    {'l', n_dright} /* 108 */,
};

void n_dtree() { 
    E.mode = MISC;
    set_cursor_type();

    int c = read_keypress();
    for (int i = 0; i < LEN(n_dmap); ++i)
        if (n_dmap[i].c == c) {
            n_dmap[i].cmd_func();
            break;
        }

    E.mode = NORMAL;
    set_cursor_type();
}

void n_dline() { delete_row(E.cy); }
void n_ddown() {
    delete_row(E.cy);
    delete_row(E.cy);
}
void n_dup() {
    delete_row(E.cy);
    move_cursor(UP);
    delete_row(E.cy);
}
void n_dleft() {
    delete_char();
}
void n_dright() {
    move_cursor(RIGHT);
    move_cursor(RIGHT);
    delete_char();
}
/*  }}} */
/* --- Yank --- {{{ */
const struct mapping n_ymap[] = {
    {'h', n_yleft}, /* 104 */
    {'j', n_ydown}, /* 106 */
    {'k', n_yup},   /* 107 */
    {'l', n_yright} /* 108 */,
    {'y', n_yline}, /* 121 */
};

void n_ytree() { 
    E.mode = MISC;
    set_cursor_type();

    int c = read_keypress();
    for (int i = 0; i < LEN(n_ymap); ++i)
        if (n_ymap[i].c == c) {
            n_ymap[i].cmd_func();
            break;
        }

    E.mode = NORMAL;
    set_cursor_type();
}

void n_yline() { 
    cpy_append(E.row[E.cy].render);
    /* E.row[1].chars = E.row[E.cy].chars; */
    /* E.row[1].render = E.row[E.cy].render; */
}
void n_ydown() {
    delete_row(E.cy);
    delete_row(E.cy);
}
void n_yup() {
    delete_row(E.cy);
    move_cursor(UP);
    delete_row(E.cy);
}
void n_yleft() {
    delete_char();
}
void n_yright() {
    move_cursor(RIGHT);
    move_cursor(RIGHT);
    delete_char();
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
    {'\r', i_return}, /* 13 */
    {27,   i_escape},
    {127,  i_backspace},
    {1000, i_move_left},
    {1001, i_move_down},
    {1002, i_move_up},
    {1003, i_move_right},
};
/* --- Functions --- {{{ */
void i_move_left() {
    move_cursor(LEFT);
    E.print_flag = 0;
}

void i_move_down() {
    move_cursor(DOWN);
    E.print_flag = 0;
}

void i_move_up() {
    move_cursor(UP);
    E.print_flag = 0;
}

void i_move_right() {
    move_cursor(RIGHT);
    E.print_flag = 0;
}

void i_null() {
    E.print_flag = 0;
    return;
}

void i_escape() {
    move_cursor(LEFT);
    E.mode = NORMAL;
    E.print_flag = 0;
    set_cursor_type();
}

void i_backspace() {
    delete_char();
    E.print_flag = 0;
}

void i_delete() {
    move_cursor(RIGHT);
    delete_char();
    E.print_flag = 0;
}

void i_return() {
    insert_nl();
    E.print_flag = 0;
}
/* }}} */
/*}}}*/
/* -- Execute -- {{{ */
/* --- Prototypes --- {{{ */
void n_exmode();
void e_quit();
void e_fquit();
void e_write();
void e_writeq();
/* }}} */
/* New struct to handle execute mode functions */
const struct mapping_exec {
    char *cmd;
    handle cmd_func;
} e_map[] = { 
    {"q", e_quit},
    {"q!", e_fquit},
    {"w", e_write},
    {"wq", e_writeq},
};
/* --- Functions --- {{{ */
void n_exmode() {
    E.mode = NORMAL;
    set_cursor_type();

    char *buf = prompt_line(":%s", NULL);
    if (buf)
        for (int i = 0; i < LEN(e_map); ++i)
            if (strcmp(buf, e_map[i].cmd)) {
                e_map[i].cmd_func();
                break;
            }
}

void e_quit() {
    if (E.dirty) {
        set_sts_msg("WARNING!! This file has been modified and not saved. "
                "Save the file and close `:wq`, or force close `:q!`");
        return;
    } else quit();
}
void e_fquit() {
    quit();
}

void e_write() {
    save_file();
}
void e_writeq() {
    save_file();
    quit();
}
/* }}} */
/*}}} */
/* -- Binary search -- {{{ */
int bin_search (const struct mapping map[], int left, int right, int x) {
    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (map[mid].c == x) return mid;
        if (map[mid].c < x)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}
/*}}}*/
/* -- Process Keypress -- {{{ */
void process_keypress () {
    /* static int quit_times = QUIT_TIMES; */
    int idx;

    int c = read_keypress();
    int mode = E.mode;

    /* Normal Mode */
    /* TODO: Sort structs and search via binary search instead of brute */
    if (mode == NORMAL) {
        /* bin search for index of keypress in n_map */
        idx = bin_search(n_map, 0, LEN(n_map)-1, c);
        if (idx != -1)
            n_map[idx].cmd_func();
    }
    if (mode == INSERT) {
        idx = bin_search(i_map, 0, LEN(i_map)-1, c);
        if (idx != -1)
            i_map[idx].cmd_func();
        if (E.mode == INSERT && E.print_flag == 1) // secondary check just incase
            insert_char(c);
    }

    E.print_flag = 1;
    /* quit_times = QUIT_TIMES; */
}
    /* if (mode == NORMAL) */
    /*     for (int i = 0; i < LEN(n_map); ++i) */
    /*         if (n_map[i].c == c) { */
    /*             n_map[i].cmd_func(); */
    /*             break; */
    /*         } */
    /* if (mode == INSERT) { */
    /*     for (int j = 0; j < LEN(i_map); ++j) */
    /*         if (i_map[j].c == c) { */
    /*             i_map[j].cmd_func(); */
    /*             break; */
    /*         } */
    /*     if (E.mode == INSERT && E.print_flag == 1) // secondary check just incase */
    /*         insert_char(c); */
    /* } */

/*     case CTRL_KEY('q'): */
/*         if (E.dirty && quit_times > 0) { */
/*             set_sts_msg("WARNING!! File has unsaved changes. " */
/*                     "Press C-q %d more times to quit", quit_times); */
/*             quit_times--; */
/*             return; */
/*         } */
/*         write(STDOUT_FILENO, "\x1b[2J", 4); */
/*         write(STDOUT_FILENO, "\x1b[H", 3); */
/*         exit(0); */
/*         break; */
/*     case CTRL_KEY('s'): */
/*         save_file(); */
/*         break; */
/*}}}*/
/* -------------------------------------------------------------------------- */
