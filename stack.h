#ifndef IFJ15_STACK_H
#define IFJ15_STACK_H

#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "helper.h"
#include "token.h"

struct stack_item {
    void *data;
    struct stack_item *next;
};

struct stack {
    struct stack_item *top;
};

struct stack *stack_init ();
void stack_push (struct stack *stack, void *data);
void stack_pop (struct stack *stack);
void stack_delete (struct stack *stack);
void stack_reverse (struct stack_item **item);

#endif //IFJ15_STACK_H