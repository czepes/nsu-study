#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#define INPUT_FILE "in.txt"
#define OUTPUT_FILE "out.txt"
#define ERROR_MSG "bad input"
#define OVERFLOW_MSG "overflow"
#define MIN_SIZE 1
#define MAX_SIZE 10

bool is_int(long long num) {
    if (num < INT_MIN || num > INT_MAX) return false;
    return true;
}

int main(void) {
    int size;
    int count = 0;
    long long num;
    long long sum = 0;

    if (freopen(INPUT_FILE, "r", stdin) == NULL) {
        fprintf(stderr, "Cannot open input file\n");
        return EXIT_FAILURE;
    }

    if (freopen(OUTPUT_FILE, "w", stdout) == NULL) {
        fprintf(stderr, "Cannot open output file\n");
        return EXIT_FAILURE;
    }

    if (scanf("%d", &size) != 1 || size < MIN_SIZE || size > MAX_SIZE) {
        printf(ERROR_MSG);
        return EXIT_SUCCESS;
    }

    while (scanf("%lld", &num) == 1) {
        if (count > size || !is_int(num)) {
            printf(ERROR_MSG);
            return EXIT_SUCCESS;
        }
        count++;
        sum += num;
        if (!is_int(sum)) {
            printf(OVERFLOW_MSG);
            return EXIT_SUCCESS;
        }
    }

    if (count < size) {
        printf(ERROR_MSG);
        return EXIT_SUCCESS;
    }

    printf("%lld\n", sum);

    return EXIT_SUCCESS;
}
