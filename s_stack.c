/* -------------------------------- s_stack.c ------------------------------- */
#include "shado.h"

int is_empty (Stack *top) {
    return !top;
}

/* Initializes and allocates a new stack node */
Stack *new_node (struct GlobalState *state) {
    Stack *new_node = (Stack*)malloc(sizeof(Stack));
    new_node->snap = state;
    new_node->next = NULL;
    return new_node;
}

/* Pushes a newly allocated node to the stack */
void push (Stack **top, struct GlobalState *state) {
    Stack *nnode = new_node(state);
    nnode->next = *top;
    *top = nnode;
}

/* Pops off stack the topmost GlobalState snapshot */
struct GlobalState *pop (Stack **top) {
    if (is_empty(*top)) return NULL;
    Stack *tmp = *top;
    *top = (*top)->next;
    struct GlobalState *pop = tmp->snap;
    free(tmp);
    return pop;
}

/* Views the topmost GlobalState snapshot */
struct GlobalState *peek (Stack *top) {
    if (is_empty(top)) return NULL;
    return top->snap;
}
/* -------------------------------------------------------------------------- */
