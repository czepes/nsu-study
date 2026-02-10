#ifndef STACK_HEADER
#define STACK_HEADER

#include "constraints.h"

typedef struct stack {
    int size;
    int content[N];
} Stack;

void initStack(Stack *stack);
void push(Stack *stack, int value);
int pop(Stack *stack);

#endif
