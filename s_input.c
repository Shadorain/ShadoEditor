/* -------------------------------- s_input.c ------------------------------- */
#include "shado.h"
// -- Keycode -- {{{
#define CR     0
#define key_a  1
#define key_b  2
#define key_c  3
#define key_d  4
#define key_e  5
#define key_f  6
#define key_g  7
#define key_h  8
#define key_i  9
#define key_j  10
#define key_k  11
#define key_l  12
#define key_m  13
#define key_n  14
#define key_o  15
#define key_p  16
#define key_q  17
#define key_r  18
#define key_s  19
#define key_t  20
#define key_u  21
#define key_v  22
#define key_w  23
#define key_x  24
#define key_y  25
#define key_z  26
#define key_A  27
#define key_B  28
#define key_C  29
#define key_D  30
#define key_E  31
#define key_F  32
#define key_G  33
#define key_H  34
#define key_I  35
#define key_J  36
#define key_K  37
#define key_L  38
#define key_M  39
#define key_N  40
#define key_O  41
#define key_P  42
#define key_Q  43
#define key_R  44
#define key_S  45
#define key_T  46
#define key_U  47
#define key_V  48
#define key_W  49
#define key_X  50
#define key_Y  51
#define key_Z  52
#define Ctrl_a 53
#define Ctrl_b 54
#define Ctrl_c 55
#define Ctrl_d 56
#define Ctrl_e 57
#define Ctrl_f 58
#define Ctrl_g 59
#define Ctrl_h 60
#define Ctrl_i 61
#define Ctrl_j 62
#define Ctrl_k 63
#define Ctrl_l 64
#define Ctrl_m 65
#define Ctrl_n 66
#define Ctrl_o 67
#define Ctrl_p 68
#define Ctrl_q 69
#define Ctrl_r 70
#define Ctrl_s 71
#define Ctrl_t 72
#define Ctrl_u 73
#define Ctrl_v 74
#define Ctrl_w 75
#define Ctrl_x 76
#define Ctrl_y 77
#define Ctrl_z 78
//}}}
typedef struct func_args {
    int arg;
} funcarg_t;

void n_append();
void n_null();
void n_return();
typedef void (*nfunc_t)(funcarg_t *args);

static const struct nmode_keys {
    int	key;
    nfunc_t cmd_func;
} ncmds[] = 
{ // -- ncmds -- {{{
    {CR,        n_return},
    {key_a,     n_append},
    {key_b,     n_null},
    {key_c,     n_null},
    {key_d,     n_null},
    {key_e,     n_null},
    {key_f,     n_null},
    {key_g,     n_null},
    {key_h,     n_null},
    {key_i,     n_null},
    {key_j,     n_null},
    {key_k,     n_null},
    {key_l,     n_null},
    {key_m,     n_null},
    {key_n,     n_null},
    {key_o,     n_null},
    {key_p,     n_null},
    {key_q,     n_null},
    {key_r,     n_null},
    {key_s,     n_null},
    {key_t,     n_null},
    {key_u,     n_null},
    {key_v,     n_null},
    {key_w,     n_null},
    {key_x,     n_null},
    {key_y,     n_null},
    {key_z,     n_null},
    {key_A,     n_null},
    {key_B,     n_null},
    {key_C,     n_null},
    {key_D,     n_null},
    {key_E,     n_null},
    {key_F,     n_null},
    {key_G,     n_null},
    {key_H,     n_null},
    {key_I,     n_null},
    {key_J,     n_null},
    {key_K,     n_null},
    {key_L,     n_null},
    {key_M,     n_null},
    {key_N,     n_null},
    {key_O,     n_null},
    {key_P,     n_null},
    {key_Q,     n_null},
    {key_R,     n_null},
    {key_S,     n_null},
    {key_T,     n_null},
    {key_U,     n_null},
    {key_V,     n_null},
    {key_W,     n_null},
    {key_X,     n_null},
    {key_Y,     n_null},
    {key_Z,     n_null},
    {Ctrl_a,    n_null},
    {Ctrl_b,    n_null},
    {Ctrl_c,    n_null},
    {Ctrl_d,    n_null},
    {Ctrl_e,    n_null},
    {Ctrl_f,    n_null},
    {Ctrl_g,    n_null},
    {Ctrl_h,    n_null},
    {Ctrl_i,    n_null},
    {Ctrl_j,    n_null},
    {Ctrl_k,    n_null},
    {Ctrl_l,    n_null},
    {Ctrl_m,    n_null},
    {Ctrl_n,    n_null},
    {Ctrl_o,    n_null},
    {Ctrl_p,    n_null},
    {Ctrl_q,    n_null},
    {Ctrl_r,    n_null},
    {Ctrl_s,    n_null},
    {Ctrl_t,    n_null},
    {Ctrl_u,    n_null},
    {Ctrl_v,    n_null},
    {Ctrl_w,    n_null},
    {Ctrl_x,    n_null},
    {Ctrl_y,    n_null},
    {Ctrl_z,    n_null},
};// }}}

void n_append() {
    editorMoveCursor(RIGHT);
    E.mode = INSERT;
}

void n_null() {
    return;
}

void n_return() {
    editorInsertNewline();
}
// -- Prompt -- {{{
char *editorPrompt (char *prompt, void (*callback)(char *, int)) {
    size_t bufsize = 128;
    char *buf = malloc(bufsize);
    size_t buflen = 0;
    buf[0] = '\0';

    while (1) {
        editorSetStatusMessage(prompt, buf);
        editorRefreshScreen();

        int c = editorReadKey();
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
            if (buflen != 0) buf[--buflen] = '\0';
        } else if (c == '\x1b') {
            editorSetStatusMessage("");
            if (callback) callback(buf, c);
            free(buf);
            return NULL;
        } else if (c == '\r') {
            if (buflen != 0) {
                editorSetStatusMessage("");
                if (callback) callback(buf, c);
                return buf;
            }
        } else if (!iscntrl(c) && c < 128) {
            if (buflen == bufsize - 1) {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }
        if (callback) callback(buf, c);
    }
}
//}}}
// -- Move Cursor -- {{{
void editorMoveCursor (int key) {
    erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

    switch (key) {
        case LEFT: //case ARROW_LEFT:
            if (E.cx != 0)
                E.cx--;
            else if (E.cy > 0) {
                E.cy--;
                E.cx = E.row[E.cy].size;
            }
            break;
        case DOWN: //case ARROW_DOWN:
            if (E.cy < E.numrows)
                E.cy++;
            break;
        case UP: //case ARROW_UP: if (E.cy != 0)
                E.cy--;
            break;
        case RIGHT: //case ARROW_RIGHT:
            if (row && E.cx < row->size)
                E.cx++;
            else if (row && E.cx == row->size) {
                E.cy++;
                E.cx = 0;
            }
            break;
    }

    row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen)
        E.cx = rowlen;
}
//}}}
// -- Process Keypress -- {{{
void editorProcessKeypress () {
    static int quit_times = QUIT_TIMES;

    int c = editorReadKey();
    int mode = E.mode;

    // Normal Mode
    if (mode == 0) {
        funcarg_t *args = malloc(sizeof(funcarg_t));
        ncmds[c].cmd_func(args);
        free(args);
        /* switch (c) { */
            /* case '\r': */
            /*     editorInsertNewline(); */
            /*     break; */

/*             case CTRL_KEY('q'): */
/*                 if (E.dirty && quit_times > 0) { */
/*                     editorSetStatusMessage("WARNING!! File has unsaved changes. " */
/*                             "Press C-q %d more times to quit", quit_times); */
/*                     quit_times--; */
/*                     return; */
/*                 } */
/*                 write(STDOUT_FILENO, "\x1b[2J", 4); */
/*                 write(STDOUT_FILENO, "\x1b[H", 3); */
/*                 exit(0); */
/*                 break; */

/*             case BACKSPACE: case CTRL_KEY('h'): //case DEL_KEY: */
/*                 if (c == DEL_KEY) editorMoveCursor(RIGHT); */
/*                 editorDelChar(); */
/*                 break; */

/*             case CTRL_KEY('s'): */
/*                 editorSave(); */
/*                 break; */

/*             case CTRL_KEY('l'): */
/*             case '\x1b': */
/*                 /1* TODO *1/ */
/*                 break; */

/*             case CTRL_KEY('f'): */
/*                 editorFind(); */
/*                 break; */

            /* case HOME_KEY: */
            /*     E.cx = 0; */
            /*     break; */
            /* case END_KEY: */
            /*     if (E.cy < E.numrows) */
            /*         E.cx = E.row[E.cy].size; */
            /*     break; */

            /* case PAGE_UP: case PAGE_DOWN: */
            /*     { */
            /*         if (c == PAGE_UP) */
            /*             E.cy = E.rowoff; */
            /*         else if (c == PAGE_DOWN) { */
            /*             E.cy = E.rowoff + E.screenrows - 1; */
            /*             if (E.cy > E.numrows) E.cy = E.numrows; */
            /*         } */

            /*         int times = E.screenrows; */
            /*         while (times--) */
            /*             editorMoveCursor(c == PAGE_UP ? UP : DOWN); */
            /*     } */
            /*     break; */

            /* case LEFT: case DOWN: case UP: case RIGHT: */
            /* case ARROW_LEFT: case ARROW_DOWN: case ARROW_UP: case ARROW_RIGHT: */
            /*     editorMoveCursor(c); */
            /*     break; */

/*             default: */
/*                 editorInsertChar(c); */
/*                 break; */
        /* } */
    }
    quit_times = QUIT_TIMES;
}
//}}}
/* -------------------------------------------------------------------------- */
