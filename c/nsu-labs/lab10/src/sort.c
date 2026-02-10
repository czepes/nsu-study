#include "sort.h"

void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int partition(int* arr, int left, int right, CompareResult (*compare) (int*, int, int)) {
    if (left >= right) return left;

    int middle = (right + left) / 2;

    for (int i = left, j = right; i < j;) {
        while (i < middle && compare(arr, i, middle) <= EQUAL) {
            i++;
        }
        while (j > middle && compare(arr, j, middle) >= EQUAL) {
            j--;
        }

        if (i == middle) middle = j;
        else if (j == middle) middle = i;

        swap(arr + i, arr + j);
    }

    return middle;
}

void quicksort(int* arr, int left, int right, CompareResult (*compare) (int*, int, int)) {
    if (left >= right) return;

    int middle = partition(arr, left, right, compare);

    quicksort(arr, left, middle - 1, compare);
    quicksort(arr, middle + 1, right, compare);
}

