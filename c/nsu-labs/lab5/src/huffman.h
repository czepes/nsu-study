#ifndef HUFFMAN_HEADER
#define HUFFMAN_HEADER

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "reader.h"
#include "minheap.h"
#include "ptree.h"
#include "bitio.h"

#define CHARS UCHAR_MAX + 1
#define CODE_LEN 128

void compress();
void decompress();

#endif
