#include "minheap.h"

MinHeap* initMinHeap(int capacity) {
    MinHeap *heap = malloc(sizeof(MinHeap) + capacity * sizeof(HeapNode));
    if (heap == NULL) return NULL;

    heap->capacity = capacity;
    heap->size = 0;

    return heap;
}

void swap(HeapNode *nodes, int i, int j) {
    HeapNode tmp = nodes[i];
    nodes[i] = nodes[j];
    nodes[j] = tmp;
}

void heapify(MinHeap *heap, int parent) {
    if (parent < 0) {
        return;
    }

    int minimal = parent;
    int left = 2 * parent + 1;
    int right = 2 * parent + 2;

    if (left < heap->size && heap->nodes[left].key < heap->nodes[minimal].key) {
        minimal = left;
    }
 
    if (right < heap->size && heap->nodes[right].key < heap->nodes[minimal].key) {
        minimal = right;
    }

    if (minimal != parent) {
        swap(heap->nodes, parent, minimal);
        heapify(heap, minimal);
    }
}

void siftup(MinHeap *heap, int child) {
    if (child <= 0 || child >= heap->size) return;

    int parent = (child - 1) / 2;

    if (heap->nodes[child].key < heap->nodes[parent].key) {
        swap(heap->nodes, child, parent);
        siftup(heap, (child - 1) / 2);
    }
}

void insert(MinHeap *heap, int value, int key) {
    if (heap->size == heap->capacity) return;

    HeapNode *node = &heap->nodes[heap->size++];

    node->key = key;
    node->value = value;

    siftup(heap, heap->size - 1);
}

int extractMin(MinHeap *heap) {
    if (heap->size < 1) return EXTRACT_FAILURE;

    int value = heap->nodes[0].value;

    heap->nodes[0] = heap->nodes[--heap->size];
    heapify(heap, 0);

    return value;
}
