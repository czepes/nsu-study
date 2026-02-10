#include <stdlib.h>
#include <stdio.h>

int find_single(int *arr, int n) {
    int result = 0;

    for (int i = 0; i < n; i++) {
        result ^= arr[i];
    }

    return result;
}

int main(void) {
    int n = 7;
    int arr[7] = {1, 1, 5, 6, 6, 3, 3};

    printf("Single element in array is %d\n", find_single(arr, n));

    return EXIT_SUCCESS;
}
