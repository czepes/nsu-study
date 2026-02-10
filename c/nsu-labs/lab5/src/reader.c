#include "reader.h"

Reader* initReader(FILE *in) {
    Reader *reader = malloc(sizeof(Reader));
    if (!reader) return NULL;

    reader->length = 0;
    reader->in = in;
    fgetpos(in, &reader->start);

    return reader;
}

int readerRead(Reader *reader) {
    reader->length = fread(
        reader->buffer,
        sizeof(*reader->buffer),
        BUFFER_SIZE, reader->in
    );
    return reader->length;
}

void readerRestart(Reader *reader) {
    fsetpos(reader->in, &reader->start);
}
