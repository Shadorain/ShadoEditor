/* <| Shado :: a lightweight, modular, and modal editor |> */
#include "shado.h"

struct globalState E;

void init () {
    E.cx = 0;
    E.cy = 0;
    E.rx = 0;
    E.numrows = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.row = NULL;
    E.filename = NULL;
    E.stsmsg[0] = '\0';
    E.stsmsg_time = 0;
    E.dirty = 0;
    E.syntax = NULL;

    E.mode = 0;
    E.print_flag = 1;
    E.cpyhead = NULL;

    if (get_win_size(&E.screenrows, &E.screencols) == -1) kill("getWindowSize");
    E.screenrows -= 2;
}

int main (int argc, char *argv[]) {
    enable_raw();
    init();
    if (argc >= 2) open_file(argv[1]);

    set_sts_msg("HELP: C-q: Quit | C-s: Save | C-f: Find");

    while (1) {
        refresh_screen();
        process_keypress();
    }
    return 0;
}
/* <| ------------------------------------------------- |> */
