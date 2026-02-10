#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool compare_byte(int num, int byte) {
    return (num & byte) == byte;
}

int* find_missing(int n) {
    int mask = 0;
    int num = 0;

    for (int i = 1; i <= n; i++) {
        if (scanf("%d", &num) != 1) {
            num = 0;
        }
        mask ^= num ^ i;
    }

    int major_byte = 1;

    while (major_byte <= mask) {
        major_byte <<= 1;
    }

    major_byte >>= 1;

    rewind(stdin);
    scanf("%d", &num);

    int *missing = calloc(2, sizeof(int));

    for (int i = 1; i <= n; i++) {
        if (scanf("%d", &num) != 1) {
            num = 0;
        }

        if (compare_byte(num, major_byte)) {
            missing[1] ^= num;
        } else {
            missing[0] ^= num;
        }

        if (compare_byte(i, major_byte)) {
            missing[1] ^= i;
        } else {
            missing[0] ^= i;
        }
    }

    return missing;
}

int main(void) {
    FILE *fp = freopen("in.txt", "rb", stdin);

    if (fp == NULL) {
        return EXIT_FAILURE;
    }

    int n;

    if (scanf("%d", &n) != 1) {
        return EXIT_FAILURE;
    }

    int *missing = find_missing(n);

    if (missing == NULL) {
        return EXIT_FAILURE;
    }

    printf("%d %d\n", missing[0], missing[1]);

    free(missing);
    fclose(fp);
    return EXIT_SUCCESS;
}
