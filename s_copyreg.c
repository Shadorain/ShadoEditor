/* ------------------------------- s_copyreg.c ------------------------------ */
#include "shado.h"

void cpy_print() {
    set_sts_msg("HEAD: %s", E.cpyhead->line);
    /* int rx = E.rx; /1* To go back to after printing *1/ */
    /* int cx = E.cx; */

    /* Extremely slow but gets the job done for now */
    for(int i = 0; i < (int)strlen(E.cpyhead->line); i++)
        insert_char(E.cpyhead->line[i]);
    /* E.rx = rx; */
    /* E.cx = cx; */
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
