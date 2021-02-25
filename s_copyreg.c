/* ------------------------------- s_copyreg.c ------------------------------ */
#include "shado.h"

void cpy_print() {
    /* /1* TODO: Traverse list when printing to print last copy done, unless... */
    /*  * append to the start of the list, and then no need to traverse: O(1) *1/ */
    /* CopyRegister *temp = E.cpyhead; */
    /* /1* write(STDOUT_FILENO, "\x1b[2J", 4); *1/ */
    /* /1* write(STDOUT_FILENO, "\x1b[H", 3); *1/ */
    /* /1* printf("List is: "); *1/ */
    /* /1* if (temp) *1/ */
    /* /1*     while(temp->next != NULL) *1/ */
    /* /1*         temp = temp->next; *1/ */

    set_sts_msg("HEAD: %s", E.cpyhead->line);
    /* printf("HEAD: %s",temp->line); */
    /* printf("\n"); */
    /* exit(0); */
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

/* Better option for O(1) inserting and retrieval */
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
