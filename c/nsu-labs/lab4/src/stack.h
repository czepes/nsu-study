#ifndef STACK_HEADER
#define STACK_HEADER

#include <stdlib.h>
#include <stdio.h>

typedef struct Element* StackElement;

struct Element {
    char op;
    int priority;
    StackElement next;
};

typedef struct {
    StackElement top;
} Stack;

StackElement pop(Stack *stack);
void push(Stack *stack, char op, int priority);
void rm(Stack *stack);
void clean(Stack *stack);
Stack* new_stack();

#endif
