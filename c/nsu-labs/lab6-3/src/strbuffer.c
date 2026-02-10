#include "strbuffer.h"

StringBuffer* initText(int capacity) {
    StringBuffer *buffer = malloc(sizeof(StringBuffer));
    if (!buffer) return NULL;

    buffer->data = malloc(sizeof(char) * capacity);
    if (!buffer->data) {
        free(buffer);
        return NULL;
    }

    buffer->capacity = capacity;
    buffer->length = 0;

    return buffer;
}

void freeText(StringBuffer *buffer) {
    if (!buffer) return;
    if (buffer->data) free(buffer->data);
    free(buffer);
}

void bufferAppend(StringBuffer *buffer, char *str, int len) {
    while (buffer->length + len > buffer->capacity) {
        char *newData = realloc(buffer->data, sizeof(char) * (buffer->capacity *= 2));

        if (!newData) {
            freeText(buffer);
            return;
        }

        buffer->data = newData;
    }

    strncpy(buffer->data + buffer->length, str, len);
    buffer->length += len;
}

