/* -------------------------------- s_term.c -------------------------------- */
#include "shado.h"

void kill (const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void disableRawMode () { // Restores on exit to terminal's orig attributes
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        kill("tcsetattr");
}

void enterRawMode () {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) kill("tcgetarr");
    atexit(disableRawMode); // at exit, do ...

    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) kill("tcsetattr");
}

int editorReadKey () {
    int nread;
    char c;
    // loop: read 1B into `c` from stdin
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
        if (nread == -1 && errno != EAGAIN) kill("read");

    if (E.mode != INSERT) {
        switch (c) {
            // -- Key change -- {{{
            case '\r': return 0;
            case 'a':  return 1;
            case 'b':  return 2;
            case 'c':  return 3;
            case 'd':  return 4;
            case 'e':  return 5;
            case 'f':  return 6;
            case 'g':  return 7;
            case 'h':  return 8;
            case 'i':  return 9;
            case 'j':  return 10;
            case 'k':  return 11;
            case 'l':  return 12;
            case 'm':  return 13;
            case 'n':  return 14;
            case 'o':  return 15;
            case 'p':  return 16;
            case 'q':  return 17;
            case 'r':  return 18;
            case 's':  return 19;
            case 't':  return 20;
            case 'u':  return 21;
            case 'v':  return 22;
            case 'w':  return 23;
            case 'x':  return 24;
            case 'y':  return 25;
            case 'z':  return 26;
            case 'A':  return 27;
            case 'B':  return 28;
            case 'C':  return 29;
            case 'D':  return 30;
            case 'E':  return 31;
            case 'F':  return 32;
            case 'G':  return 33;
            case 'H':  return 34;
            case 'I':  return 35;
            case 'J':  return 36;
            case 'K':  return 37;
            case 'L':  return 38;
            case 'M':  return 39;
            case 'N':  return 40;
            case 'O':  return 41;
            case 'P':  return 42;
            case 'Q':  return 43;
            case 'R':  return 44;
            case 'S':  return 45;
            case 'T':  return 46;
            case 'U':  return 47;
            case 'V':  return 48;
            case 'W':  return 49;
            case 'X':  return 50;
            case 'Y':  return 51;
            case 'Z':  return 52;
            case CTRL_KEY('a'):  return 53;
            case CTRL_KEY('b'):  return 54;
            case CTRL_KEY('c'):  return 55;
            case CTRL_KEY('d'):  return 56;
            case CTRL_KEY('e'):  return 57;
            case CTRL_KEY('f'):  return 58;
            case CTRL_KEY('g'):  return 59;
            case CTRL_KEY('h'):  return 60;
            case CTRL_KEY('i'):  return 61;
            case CTRL_KEY('j'):  return 62;
            case CTRL_KEY('k'):  return 63;
            case CTRL_KEY('l'):  return 64;
            /* case CTRL_KEY('m'):  return 65; */ // Alt to CR
            case CTRL_KEY('n'):  return 66;
            case CTRL_KEY('o'):  return 67;
            case CTRL_KEY('p'):  return 68;
            case CTRL_KEY('q'):  return 69;
            case CTRL_KEY('r'):  return 70;
            case CTRL_KEY('s'):  return 71;
            case CTRL_KEY('t'):  return 72;
            case CTRL_KEY('u'):  return 73;
            case CTRL_KEY('v'):  return 74;
            case CTRL_KEY('w'):  return 75;
            case CTRL_KEY('x'):  return 76;
            case CTRL_KEY('y'):  return 77;
            case CTRL_KEY('z'):  return 78;
            default:   return c;
            // }}}
        }
    } else
        return c;
    /* if (c == '\x1b') { */
    /*     char seq[3]; */
    /*     if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b'; */
    /*     if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b'; */

    /*     if (seq[0] == '[') { */
    /*         if (seq[1] >= '0' && seq[1] <= '9') { */
    /*             if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b'; */
    /*             if (seq[2] == '~') */
    /*                 switch (seq[1]) { */
    /*                     case '1': return HOME_KEY; */
    /*                     case '3': return DEL_KEY; */
    /*                     case '4': return END_KEY; */
    /*                     case '5': return PAGE_UP; */
    /*                     case '6': return PAGE_DOWN; */
    /*                     case '7': return HOME_KEY; */
    /*                     case '8': return END_KEY; */
    /*                 } */
    /*         } else */
    /*             switch (seq[1]) { */
    /*                 case 'A': return LEFT; */
    /*                 case 'B': return UP; */
    /*                 case 'C': return DOWN; */
    /*                 case 'D': return RIGHT; */
    /*                 case 'H': return HOME_KEY; */
    /*                 case 'F': return END_KEY; */
    /*             } */
    /*     } else if (seq[0] == '0') */
    /*         switch (seq[1]) { */
    /*             case 'H': return HOME_KEY; */
    /*             case 'F': return END_KEY; */
    /*         } */
    /*     return '\x1b'; */
    /* } else */
}

int getCursorPosition (int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    while  (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    printf("\r\n&buf[1]: '%s'\r\n", &buf[1]);

    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

    return 0;
}

int getWindowSize (int *rows, int *cols) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return getCursorPosition(rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}
/* -------------------------------------------------------------------------- */
