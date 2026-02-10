#include <stdlib.h>
#include <stdio.h>

void swap(int* arr, int i, int j) {
    if (i == j) return;
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
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

int bs_upper(int value, int *arr, int size) {
    int l = 0, m, r = size;

    while (r - l > 1) {
        m = (l + r) / 2;

        if (value >= arr[m]) {
            l = m;
        } else {
            r = m;
        }
    }

    if (l == 0 && arr[l] > value) {
        return -1;
    }

    return l;
}

int bs_lower(int value, int *arr, int size) {
    int l = 0, m, r = size;

    while (r - l > 1) {
        m = (l + r) / 2;

        if (value > arr[m]) {
            l = m;
        } else {
            r = m;
        }
    }

    if (r == size && arr[l] < value) {
        return size;
    }

    return arr[l] >= value ? l : r;
}

int fast_search(int *arr, int size, int l, int r) {
    return bs_upper(r, arr, size) - bs_lower(l, arr, size) + 1;
}

int main(void) {
    int size;

    if (scanf("%d", &size) != 1) {
        return EXIT_FAILURE;
    }

    int arr[size];

    for (int i = 0; i < size; i++) {
        if (scanf("%d", arr + i) != 1) {
            return EXIT_FAILURE;
        }
    }

    heap_sort(arr, size);

    int cases;

    if (scanf("%d", &cases) != 1) {
        return EXIT_FAILURE;
    }

    for (int test = 0; test < cases; test++) {
        int l, r;

        if (scanf("%d %d", &l, &r) != 2) {
            break;
        }

        printf("%d\n", fast_search(arr, size, l, r));
    }

    return EXIT_SUCCESS;
}
