#include "stack.h"
#include <stdio.h>

StackElement pop(Stack *stack) {
    StackElement popped = stack->top;

    if (popped == NULL) return popped;

    stack->top = popped->next;

    return popped;
}

void push(Stack *stack, char op, int priority) {
    StackElement pushed = malloc(sizeof(struct Element));

    if (pushed == NULL) return;

    pushed->op = op;
    pushed->priority = priority;

    pushed->next = stack->top;
    stack->top = pushed;
}

void rm(Stack *stack) {
    if (stack->top != NULL) {
        free(pop(stack));
    }
}

void clean(Stack *stack) {

    while (stack->top != NULL) {
        free(pop(stack));
    }

    free(stack);
}

Stack* new_stack(void) {
    Stack *stack = malloc(sizeof(Stack));

    if (stack != NULL) stack->top = NULL;

    return stack;
}
