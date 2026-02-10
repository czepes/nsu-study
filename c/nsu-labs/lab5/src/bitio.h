#ifndef BITS_HEADER
#define BITS_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

typedef struct {
    FILE *out;
    int pos;
    char byte;
} BitWriter;

BitWriter* initBitWriter(FILE *out);
void writeBit(BitWriter *writer, bool bit);
void writeByte(BitWriter *writer, char byte);
char flushBits(BitWriter *writer);

typedef enum {
    READING,
    IN_TRAIL,
    FINISHED
} ReadState;

typedef struct {
    FILE *in;
    int pos;
    char byte;
    char next;
    char trail;
    ReadState state;
} BitReader;

BitReader* initBitReader(FILE *in, char trail);
bool readBit(BitReader *reader);
char readByte(BitReader *reader);
void dumpByte(BitReader *reader);
bool readerFinished(BitReader *reader);

#endif
