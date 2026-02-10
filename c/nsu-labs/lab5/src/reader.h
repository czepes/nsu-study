#ifndef READER_HEADER
#define READER_HEADER

#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

typedef struct {
    unsigned char buffer[BUFFER_SIZE];
    fpos_t start;
    int length;
    FILE *in;
} Reader;

Reader* initReader(FILE *in);
int readerRead(Reader *reader);
void readerRestart(Reader *reader);

#endif
