#include "cyclebuffer.h"

// shifts cb->end
void write(CycleBuffer *cb) {
    if (cb->input == NULL) {
        return;
    }

    int to_read = cb->capacity - cb->size;

    int bytes_read = fread(
        cb->buffer + cb->end,
        sizeof(char),
        to_read,
        cb->input
    );

    if (bytes_read < to_read) {
        cb->input = NULL;
    }

    cb->end = (cb->end + bytes_read) % cb->capacity;
    cb->size += bytes_read;
}

char read(CycleBuffer *cb, int index) {
    if (index >= cb->size) {
        return '\0';
    }

    index = (cb->start + index) % cb->capacity;

    return cb->buffer[index];
}

// shifts cb->start
/*char extract(CycleBuffer *cb) {*/
/*    if (cb->size == 0) {*/
/*        return '\0';*/
/*    }*/
/**/
/*    char ch = cb->buffer[cb->start];*/
/**/
/*    cb->start = (cb->start + 1) % cb->capacity;*/
/*    cb->size -= 1;*/
/**/
/*    return ch;*/
/*}*/

void unwrite(CycleBuffer *cb, int amount) {
    if (amount > cb->size) {
        amount = cb->size;
    }

    cb->start = (cb->start + amount) % cb->capacity;
    cb->size -= amount;
}

CycleBuffer new_cyclebuffer(int capacity, FILE *input) {
    CycleBuffer cb = {
        .start = 0,
        .end = 0,
        .size = 0,
        .capacity = capacity,
        .buffer = malloc(capacity * sizeof(char)),
        .input = input,
    };

    return cb;
}
