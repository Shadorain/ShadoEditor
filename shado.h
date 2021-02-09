/* -------------------------------- shado.h --------------------------------- */
// -- Includes -- {{{
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
//}}}
// -- Externs -- {{{
extern struct globalState E;
//}}}
// -- Macros -- {{{
#define SHADO_VERSION "0.0.1"

#define TAB_STOP 4
#define SHOW_BAR 1
#define QUIT_TIMES 1

#define HL_HIGHLIGHT_NUMS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

#define CTRL_KEY(k) ((k) & 0x1f)
#define HLDB_ENTRIES (sizeof(HLDB)) / sizeof(HLDB[0])

// Modes
#define NORMAL 0
#define INSERT 1
#define VISUAL 2
#define REPLACE 3
//}}}
// -- Data -- {{{
// --- Append buffer --- {{{
struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT { NULL, 0 }
// }}}
// --- Row --- {{{
typedef struct erow {
    int idx;
    int size;
    int rsize;
    int hl_open_comment;
    char *chars;
    char *render;
    unsigned char *hl;
} erow;
// }}}
// --- Global State --- {{{
struct globalState {
    int cx, cy;
    int rx;
    int rowoff;
    int coloff;
    int screenrows;
    int screencols;
    int numrows;
    int dirty;
    char *filename;
    char stsmsg[80];
    time_t stsmsg_time;
    struct editorSyntax *syntax;
    struct termios orig_termios;
    erow *row;

    /* 0: normal, 1: insert, 2: visual, 3: visual_line,
     * 4: visual_blk, 5: sreplace, 6: mrerplace */
    int mode;
};
// }}}
// --- Syntax --- {{{
struct editorSyntax {
    char *filetype;
    char **filematch;
    char **keywords;
    char *singleline_comment_start;
    char *multiline_comment_start;
    char *multiline_comment_end;
    int flags;
};

enum editorHighlight {
    HL_NORMAL = 0,
    HL_COMMENT,
    HL_MLCOMMENT,
    HL_KEYWORD1,
    HL_KEYWORD2,
    HL_NUMBER,
    HL_STRING,
    HL_MATCH,
};
// }}}
// --- Keys --- {{{
#define LEFT 8
#define DOWN 10
#define UP 11
#define RIGHT 12
#define ARROW_LEFT 8
#define ARROW_DOWN 10
#define ARROW_UP 11
#define ARROW_RIGHT 12
#define DEL_KEY 53
#define BACKSPACE 53
#define HOME_KEY 53
#define END_KEY 53
#define PAGE_UP 53
#define PAGE_DOWN 53
/* enum ARROW_editorKey { */
/*     BACKSPACE = 127, */
/*     LEFT = 'h', */
/*     DOWN = 'j', */
/*     UP = 'k', */
/*     RIGHT = 'l', */
/*     ARROW_LEFT = 1000, */
/*     ARROW_DOWN, */
/*     ARROW_UP, */
/*     ARROW_RIGHT, */
/*     DEL_KEY, */
/*     HOME_KEY, */
/*     END_KEY, */
/*     PAGE_UP, */
/*     PAGE_DOWN, */
/* }; */
// }}}
//}}}
// -- Prototypes -- {{{
void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();
char *editorPrompt(char *prompt, void (*callback)(char *, int));

/* |>- s_abuf.c -<| */
void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);

/* |>- s_synhl.c -<| */
void editorUpdateSyntax (erow *row);
int editorSyntaxToColor (int hl);
void editorSelectSyntaxHighlight ();

/* |>- s_term.c -<| */
void kill (const char *s);
void disableRawMode ();
void enterRawMode ();
int editorReadKey ();
int getCursorPosition (int *rows, int *cols);
int getWindowSize (int *rows, int *cols);

/* |>- s_rows.c -<| */
int editorRowCxToRx (erow *row, int cx);
int editorRowRxToCx (erow *row, int rx);
void editorUpdateRow (erow *row);
void editorInsertRow (int at, char *s, size_t len);
void editorFreeRow (erow *row);
void editorDelRow (int at);
void editorRowInsertChar (erow *row, int at, int c);
void editorRowDelChar(erow *row, int at);
void editorRowAppendString (erow *row, char *s, size_t len);

/* |>- s_ops.c -<| */
void editorInsertChar (int c);
void editorDelChar ();
void editorInsertNewline ();

/* |>- s_bar.c -<| */
void editorDrawStatusBar (struct abuf *ab);
void editorDrawMessageBar (struct abuf *ab);
void editorSetStatusMessage (const char *fmt, ...);

/* |>- s_io.c -<| */
char *editorRowsToString (int *buflen);
void editorOpen (char *filename);
void editorSave ();

/* |>- s_search.c -<| */
void editorFindCallback (char *query, int key);
void editorFind ();

/* |>- s_input.c -<| */
char *editorPrompt (char *prompt, void (*callback)(char *, int));
void editorMoveCursor (int key);
void editorProcessKeypress ();

/* |>- s_output.c -<| */
void editorScroll ();
void editorDrawRows (struct abuf *ab);
void editorRefreshScreen ();
//}}}
/* -------------------------------------------------------------------------- */
