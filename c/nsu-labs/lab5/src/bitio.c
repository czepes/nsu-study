#include "bitio.h"

BitWriter* initBitWriter(FILE *out) {
    BitWriter *writer = malloc(sizeof(BitWriter));
    if (!writer) return NULL;

    writer->out = out;
    writer->pos = CHAR_BIT;
    writer->byte = 0;

    return writer;
}

void writeBit(BitWriter *writer, bool bit) {
    writer->byte ^= (bit << --writer->pos);
    
    if (writer->pos == 0) {
        flushBits(writer);
    }
}

void writeByte(BitWriter *writer, char byte) {
    for (int pos = CHAR_BIT; pos > 0; pos--) {
        writeBit(writer, byte >> (pos - 1) & 1);
    }
}

char flushBits(BitWriter *writer) {
    if (writer->pos == CHAR_BIT) return 0;

    fprintf(writer->out, "%c", writer->byte);
    char trail = writer->pos;

    writer->byte = 0;
    writer->pos = CHAR_BIT;

    return trail;
}

void scanByte(BitReader *reader) {
    if (!reader || reader->state == FINISHED) return;

    reader->byte = reader->next;

    if (fread(&reader->next, sizeof(reader->next), 1, reader->in) != 1) {
        reader->state = IN_TRAIL;
    }
}

BitReader* initBitReader(FILE *in, char trail) {
    BitReader *reader = malloc(sizeof(BitReader));
    if (!reader) return NULL;

    reader->in = in;
    reader->pos = 0;
    reader->byte = 0;
    reader->next = 0;
    reader->trail = trail;
    reader->state = READING;

    scanByte(reader);

    return reader;
}

bool readBit(BitReader *reader) {
    if (!reader || reader->state == FINISHED) {
        return false;
    }

    if (reader->pos == 0) {
        scanByte(reader);
        reader->pos = CHAR_BIT;
    }

    bool bit = (reader->byte >> (--reader->pos)) & 1;
    
    if (reader->state == IN_TRAIL && ++reader->trail == CHAR_BIT) {
        reader->state = FINISHED;
    }

    return bit;
}

char readByte(BitReader *reader) {
    char byte = 0;
    for (int i = 0; i < CHAR_BIT; i++) {
        byte = (byte << 1) | (readBit(reader) ? 1 : 0);
    }
    return byte;
}

void dumpByte(BitReader *reader) {
    while (reader->pos > 0) {
        readBit(reader);
    }
}

bool readerFinished(BitReader *reader) {
    return reader->state == FINISHED;
}
