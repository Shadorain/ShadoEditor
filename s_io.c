/* --------------------------------- s_io.c --------------------------------- */
#include "shado.h"

char *editorRowsToString (int *buflen) {
    int tlen = 0;
    int i;
    for (i = 0; i < E.numrows; i++)
         tlen += E.row[i].size + 1;
    *buflen = tlen;

    char *buf = malloc(tlen);
    char *p = buf;
    for (i = 0; i <E.numrows; i++) {
        memcpy(p, E.row[i].chars, E.row[i].size);
        p += E.row[i].size;
        *p = '\n';
        p++;
    }
    return buf;
}

void editorOpen (char *filename) {
    free(E.filename);
    E.filename = strdup(filename);

    editorSelectSyntaxHighlight();

    FILE *fp = fopen(filename, "r");
    if (!fp) kill("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        editorInsertRow(E.numrows, line, linelen);
    }
    free(line);
    fclose(fp);
    E.dirty = 0;
}

void editorSave () {
    if (E.filename == NULL) {
        E.filename = editorPrompt("Save as: %s (ESC: Cancel)", NULL);
        if (E.filename == NULL) {
            editorSetStatusMessage("Save aborted");
            return;
        }
        editorSelectSyntaxHighlight();
    }

    int len;
    char *buf = editorRowsToString(&len);

    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        if (ftruncate(fd, len) != -1)
            if (write(fd, buf, len) == len) {
                close(fd);
                editorSetStatusMessage("%d bytes written to disk", len);
                free(buf);
                E.dirty = 0;
                return;
            }
        close(fd);
    }
    free(buf);
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}
/* -------------------------------------------------------------------------- */
