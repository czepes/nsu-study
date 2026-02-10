#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int read_arr(int *arr, int size) {

    for (int i = 0; i < size; i++) {
        if (scanf("%d", arr + i) != 1) {
            return i;
        }
    }

    return size;
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

int main(void) {
    int size, tests;

    if (scanf("%d %d", &size, &tests) != 2) {
        return EXIT_FAILURE;
    }

    int arr[size];

    if (read_arr(arr, size) != size) {
        return EXIT_FAILURE;
    }

    for (int test = 0; test < tests; test++) {
        int num;

        if (scanf("%d", &num) != 1) {
            return EXIT_FAILURE;
        }

        printf(
            "%d\n", 
            bs_lower(num, arr, size) + 1
        );
    }

    return EXIT_SUCCESS;
}
