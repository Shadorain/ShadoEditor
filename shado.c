/* <| Shado :: a lightweight, modular, and modal editor |> */
#include "shado.h"

struct globalState E;

void initEditor () {
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

    if (getWindowSize(&E.screenrows, &E.screencols) == -1) kill("getWindowSize");
    E.screenrows -= 2;
}

int main (int argc, char *argv[]) {
    enterRawMode();
    initEditor();
    if (argc >= 2) editorOpen(argv[1]);

    editorSetStatusMessage("HELP: C-q: Quit | C-s: Save | C-f: Find");

    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
}
/* <| ------------------------------------------------- |> */
