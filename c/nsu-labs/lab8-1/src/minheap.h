#ifndef MINHEAP_HEADER
#define MINHEAP_HEADER

#include <stdlib.h>

#define EXTRACT_FAILURE -1

typedef struct {
    int key;
    int value;
} HeapNode;

typedef struct {
    int size;
    int capacity;
    HeapNode nodes[];
} MinHeap;

MinHeap* initMinHeap(int capacity);
void insert(MinHeap *heap, int value, int key);
int extractMin(MinHeap *heap);

#endif
