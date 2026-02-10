#include "pqueue.h"

PQueue initPQueue() {
    PQueue queue = {
        .size = 0,
        .head = NO_NODE,
    };

    return queue;
}

int initPQueueItem(PQueue *queue, int key, int value, int next) {
    PQueueItem *item = &queue->items[queue->size];

    item->key = key;
    item->value = value;
    item->next = next;

    return queue->size++;
}

void push(PQueue *queue, int key, int value) {
    int prev = NO_NODE;
    int next = queue->head;

    while (next != NO_NODE && (value == OVERFLOW || value > queue->items[next].value)) {
        prev = next;
        next = queue->items[next].next;
    }

    int current = initPQueueItem(queue, key, value, next);

    if (prev == NO_NODE) queue->head = current;
    else queue->items[prev].next = current;
}

int pull(PQueue *queue) {
    if (queue->head == NO_NODE) {
        return NO_NODE;
    }

    int pulled = queue->head;
    queue->head = queue->items[pulled].next;

    return queue->items[pulled].key;
}

