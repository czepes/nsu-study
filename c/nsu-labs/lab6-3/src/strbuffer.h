#ifndef STRINGBUFFER_HEADER
#define STRINGBUFFER_HEADER

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    int capacity;
    int length;
} StringBuffer;

StringBuffer* initText(int capacity);
void freeText(StringBuffer *buffer);
void bufferAppend(StringBuffer *buffer, char *str, int len);

#endif
