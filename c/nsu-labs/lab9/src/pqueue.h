#ifndef PQUEUE_HEADER
#define PQUEUE_HEADER

#include "constraints.h"

typedef struct {
    int key;
    int value;
    int next;
} PQueueItem;

typedef struct {
    int size;
    int head;
    PQueueItem items[N];
} PQueue;

PQueue initPQueue();
int initPQueueItem(PQueue *queue, int key, int value, int next);
void push(PQueue *queue, int key, int value);
int pull(PQueue *queue);

#endif
