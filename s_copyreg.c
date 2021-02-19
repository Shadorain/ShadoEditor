/* ------------------------------- s_copyreg.c ------------------------------ */
#include "shado.h"

void cpy_print() {
    CopyRegister *temp = E.cpyhead;
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    printf("List is: ");
    while(temp != NULL) {
        printf("%s",temp->line);
        temp = temp->next;
    }
    printf("\n");
    exit(0);
}

void cpy_append (char *line) {
    CopyRegister *new_node = (CopyRegister*)malloc(sizeof(CopyRegister));
    CopyRegister *last = E.cpyhead;

    new_node->line = line;
    new_node->next = NULL;

    if (E.cpyhead == NULL) {
        E.cpyhead = new_node;
        return;
    }  

    while (last->next != NULL)
        last = last->next;

    last->next = new_node;
    E.cpycurr = last->next;
}
/* -------------------------------------------------------------------------- */
