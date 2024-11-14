#include "stack.h"

struct stack *stack_init () {
    struct stack *stack = (struct stack *)malloc (sizeof (struct stack));
    if (stack == NULL)
        error (INTERNAL_ERROR);

    stack->top = NULL;

    return stack;
}

void stack_push (struct stack *stack, void *data) {
    struct stack_item *item = (struct stack_item *)malloc (sizeof (struct stack_item));
    if (item == NULL)
        error (INTERNAL_ERROR);

    item->data = data;
    item->next = stack->top;

    stack->top = item;
}

void stack_pop (struct stack *stack) {
    struct stack_item *item = stack->top;

    if (item == NULL)
        error (INTERNAL_ERROR);

    stack->top = item->next;

    free (item);
}

void stack_delete (struct stack *stack) {
    while (stack->top != NULL)
        stack_pop (stack);
    free (stack);
}

void stack_reverse (struct stack_item **item) {
    struct stack_item *temp, *prev;

    if (*item == NULL)
        return;
    if ((*item)->next == NULL)
        return;

    if ((*item)->next->next == NULL) {
        (*item)->next->next = *item;
        *item = (*item)->next;
        (*item)->next->next = NULL;
    } else {
        prev = *item;
        temp = (*item)->next;
        *item = (*item)->next->next;
        prev->next = NULL;

        while ((*item)->next != NULL) {
            temp->next = prev;
            prev = temp;
            temp = *item;
            *item = (*item)->next;
        }

        temp->next = prev;
        (*item)->next = temp;
    }
}