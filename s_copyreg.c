/* ------------------------------- s_copyreg.c ------------------------------ */
#include "shado.h"

void cpy_print() {
    /* TODO: Traverse list when printing to print last copy done, unless...
     * append to the start of the list, and then no need to traverse: O(1) */
    CopyRegister *temp = E.cpyhead;
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    printf("List is: ");
    if (temp)
        while(temp->next != NULL)
            temp = temp->next;

    printf("%s",temp->line);
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

void cpy_prepend (char *line) {
    CopyRegister *new_node = (CopyRegister*)malloc(sizeof(CopyRegister));

    new_node->line = line;

    if (E.cpyhead == NULL) {
        new_node->next = NULL;
        E.cpyhead = new_node;
        return;
    }  

    new_node->next = E.cpyhead;
    E.cpyhead = new_node;
}
/* -------------------------------------------------------------------------- */
