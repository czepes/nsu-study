#include <stdlib.h>
#include <stdio.h>

#define INPUT_FAILURE -1

int input_arr(int** arr_ptr) {
    int size;

    if (scanf("%d", &size) != 1) {
        return INPUT_FAILURE;
    }

    *arr_ptr = malloc(size * sizeof(int));
    int* arr = *arr_ptr;

    if (arr == NULL) {
        return INPUT_FAILURE;
    }

    for (int i = 0; i < size; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            break;
        }
    }

    return size;
}

void swap(int* arr, int i, int j) {
    if (i == j) return;
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

void print_arr(int* arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

void heapify(int* arr, int last, int parent) {
    if (parent < 0) {
        return;
    }

    int min_idx = parent;
    int left = 2 * parent + 1;
    int right = 2 * parent + 2;

    if (left <= last && arr[left] > arr[min_idx]) {
        min_idx = left;
    }

    if (right <= last && arr[right] > arr[min_idx]) {
        min_idx = right;
    }

    if (min_idx != parent) {
        swap(arr, parent, min_idx);
        heapify(arr, last, min_idx);
    }
}

void heap_sort(int* arr, int size) {
    int last = size - 1;

    for (int i = (last - 1) / 2; i >= 0; i--) {
        heapify(arr, last, i);
    }

    while (last > 0) {
        swap(arr, 0, last);
        heapify(arr, --last, 0);
    }
}

int main(void) {
    int* arr = NULL;
    int size = input_arr(&arr);

    if (size == INPUT_FAILURE) {
        return EXIT_FAILURE;
    }

    heap_sort(arr, size);
    print_arr(arr, size);

    free(arr);
    return EXIT_SUCCESS;
}
