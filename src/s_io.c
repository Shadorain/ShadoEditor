/* --------------------------------- s_io.c --------------------------------- */
#include "shado.h"

char *rows_to_string (int *buflen) {
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

void open_file (char *filename) {
    free(E.filename);
    E.filename = strdup(filename);
    /* free(filename); */

    /* select_syntax_hl(); */

    FILE *fp = fopen(filename, "r");
    if (!fp) kill("fopen");

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
            linelen--;
        insert_row(E.numrows, line, linelen);
    }
    free(line);
    fclose(fp);
    E.dirty = 0;
}

void save_file () {
    if (E.filename == NULL) {
        E.filename = prompt_line("Save as: %s (ESC: Cancel)", NULL);
        if (E.filename == NULL) {
            set_sts_msg("Save aborted");
            return;
        }
        /* select_syntax_hl(); */
    }

    int len;
    char *buf = rows_to_string(&len);

    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        if (ftruncate(fd, len) != -1)
            if (write(fd, buf, len) == len) {
                close(fd);
                set_sts_msg("%d bytes written to disk", len);
                free(buf);
                E.dirty = 0;
                return;
            }
        close(fd);
    }
    free(buf);
    set_sts_msg("Can't save! I/O error: %s", strerror(errno));
}
/* -------------------------------------------------------------------------- */
