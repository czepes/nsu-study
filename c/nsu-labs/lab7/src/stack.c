#include "stack.h"

void initStack(Stack *stack) {
    stack->size = 0;
}

void push(Stack *stack, int value) {
    stack->content[stack->size++] = value;
}

int pop(Stack *stack) {
    if (stack->size <= 0) return 0;
    return stack->content[--stack->size];
}
