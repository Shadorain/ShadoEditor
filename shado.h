/* -------------------------------- shado.h --------------------------------- */
/* -- Includes -- {{{ */
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
/*}}}*/
/* -- Externs -- {{{ */
extern struct globalState E;
/*}}}*/
/* -- Macros -- {{{ */
#define SHADO_VERSION "0.0.1"

#define TAB_STOP 4
#define SHOW_BAR 1
#define QUIT_TIMES 1

#define HL_HIGHLIGHT_NUMS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)

#define CTRL_KEY(k) ((k) & 0x1f)
#define LEN(v) (int)(sizeof(v) / sizeof(*v))
#define HLDB_ENTRIES (sizeof(HLDB)) / sizeof(HLDB[0])

/* Modes */
#define NORMAL 0
#define INSERT 1
#define VISUAL 2
#define REPLACE 3
#define MISC 10
/*}}}*/
/* -- Data -- {{{ */
/* --- Append buffer --- {{{ */
struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT { NULL, 0 }
/* }}} */
/* --- Row --- {{{ */
typedef struct erow {
    int idx;
    int size;
    int rsize;
    int hl_open_comment;
    char *chars;
    char *render;
    unsigned char *hl;
} erow;
/* }}} */
/* --- Mapping ---  {{{ */
typedef void (*handle)(void);

struct mapping {
    int c;
    handle cmd_func;
};
/* }}} */
/* --- Copy Register --- {{{ */
struct copyRegister {
    /*TODO: linked list*/
    char *store;
    struct copyRegister *next;
};
/* }}} */
/* --- Global State --- {{{ */
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
     * 4: visual_blk, 5: sreplace, 6: mrerplace, 10: misc */
    int mode;

    /* Makes sure not to print escape code keys */
    int print_flag;

    /* Global copy register */
    struct copyRegister cpyreg;
};
/* }}} */
/* --- Syntax --- {{{ */
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
/* }}} */
/* --- Keys --- {{{ */
#define LEFT 'h'
#define DOWN 'j'
#define UP 'k'
#define RIGHT 'l'
/* #define ARROW_LEFT 1000 */
/* #define ARROW_DOWN 1001 */
/* #define ARROW_UP 1002 */
/* #define ARROW_RIGHT 1003 */
/* #define DEL_KEY 53 */
/* #define BACKSPACE 53 */
/* #define HOME_KEY 53 */
/* #define END_KEY 53 */
/* #define PAGE_UP 53 */
/* #define PAGE_DOWN 53 */
enum ARROW_editorKey {
    BACKSPACE = 127,
    /* LEFT = 'h', */
    /* DOWN = 'j', */
    /* UP = 'k', */
    /* RIGHT = 'l', */
    ARROW_LEFT = 1000,
    ARROW_DOWN = 1001,
    ARROW_UP = 1002,
    ARROW_RIGHT = 1003,
    DEL_KEY = 1004,
    HOME_KEY = 1005,
    END_KEY = 1006,
    PAGE_UP = 1007,
    PAGE_DOWN = 1008,
};
/* }}} */
/*}}}*/
/* -- Prototypes -- {{{ */
void set_sts_msg(const char *fmt, ...);
void refresh_screen();
char *prompt_line(char *prompt, void (*callback)(char *, int));

/* |>- s_abuf.c -<| */
void ab_append(struct abuf *ab, const char *s, int len);
void ab_free(struct abuf *ab);

/* |>- s_synhl.c -<| */
void update_syntax (erow *row);
int syntax_to_color (int hl);
void select_syntax_hl ();

/* |>- s_term.c -<| */
void kill (const char *s);
void disable_raw ();
void enable_raw ();
int read_keypress ();
int get_curs_pos (int *rows, int *cols);
int get_win_size (int *rows, int *cols);

/* |>- s_rows.c -<| */
int row_cx_to_rx (erow *row, int cx);
int row_rx_to_cx (erow *row, int rx);
void update_row (erow *row);
void insert_row (int at, char *s, size_t len);
void free_row (erow *row);
void delete_row (int at);
void insert_char_row (erow *row, int at, int c);
void delete_char_row(erow *row, int at);
void append_string_row (erow *row, char *s, size_t len);

/* |>- s_ops.c -<| */
void insert_char (int c);
void delete_char ();
void insert_nl ();

/* |>- s_bar.c -<| */
void draw_sts_bar (struct abuf *ab);
void draw_msg_bar (struct abuf *ab);
void set_sts_msg (const char *fmt, ...);

/* |>- s_io.c -<| */
char *rows_to_string (int *buflen);
void open_file (char *filename);
void save_file ();

/* |>- s_search.c -<| */
void search_callback (char *query, int key);
void search ();

/* |>- s_input.c -<| */
char *prompt_line (char *prompt, void (*callback)(char *, int));
void move_cursor (int key);

/* |>- s_output.c -<| */
void scroll ();
void draw_rows (struct abuf *ab);
void refresh_screen ();
void set_cursor_type ();

/* |>- s_modes.c -<| */
void process_keypress ();
/*}}}*/
/* -------------------------------------------------------------------------- */
