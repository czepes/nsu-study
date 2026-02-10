#ifndef CYCLEBUFFER_HEADER
#define CYCLEBUFFER_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int start;
    int end;
    int size;
    int capacity;
    char *buffer;
    FILE *input;
} CycleBuffer;

CycleBuffer new_cyclebuffer(int capacity, FILE *input);
void write(CycleBuffer *cb);
char read(CycleBuffer *cb, int index);
char extract(CycleBuffer *cb);

#endif
