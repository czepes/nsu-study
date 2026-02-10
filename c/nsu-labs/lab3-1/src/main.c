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

int partition(int* arr, int left, int right) {
    if (left >= right) return left;

    int middle = (right + left) / 2;
    int pivot = arr[middle];

    for (int i = left, j = right; i < j;) {
        while (i < middle && arr[i] <= pivot) {
            i++;
        }
        while (j > middle && arr[j] >= pivot) {
            j--;
        }

        if (i == middle) middle = j;
        else if (j == middle) middle = i;

        swap(arr, i, j);
    }

    return middle;
}

void quicksort(int* arr, int left, int right) {
    if (left >= right) return;

    int middle = partition(arr, left, right);

    quicksort(arr, left, middle - 1);
    quicksort(arr, middle + 1, right);
}

int main(void) {
    int* arr = NULL;
    int size = input_arr(&arr);

    if (size == INPUT_FAILURE) {
        return EXIT_FAILURE;
    }

    quicksort(arr, 0, size - 1);
    print_arr(arr, size);

    free(arr);
    return EXIT_SUCCESS;
}
