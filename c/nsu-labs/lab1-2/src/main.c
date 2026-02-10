#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cyclebuffer.h"

#define PATTERN_LENGTH 16 + 2
#define BUFFER_LENGTH 1024

typedef struct {
    char line[PATTERN_LENGTH];
    int length;
} Pattern;

int *init_kmp(Pattern *pattern) {
    int *shiftable = calloc(pattern->length, sizeof(int));

    if (shiftable == NULL) {
        return shiftable;
    }

    int j = 0;

    for (int i = 1; i < pattern->length; i++) {

        while (j > 0 && pattern->line[i] != pattern->line[j]) {
            j = shiftable[j - 1];
        }

        if (pattern->line[i] == pattern->line[j]) {
            j++;
        }

        shiftable[i] = j;
    }

    return shiftable;
}


void kmp(Pattern *pattern, CycleBuffer *cb) {
    int *prefix = init_kmp(pattern);

    if (prefix == NULL) {
        return;
    }

    for (int i = 0; i < pattern->length; i++) {
        printf("%d ", prefix[i]);
    }
    printf("\n");

    if (cb->size == 0) {
        write(cb);
    }

    int pos = 1, pi = 0;

    while (cb->size > 0 && cb->size >= pattern->length - pi) {

        if (cb->size * 2 <= cb->capacity) {
            write(cb);
        }

        char ch = extract(cb);

        while (pi > 0 && ch != pattern->line[pi]) {
            printf("%d %d ", pos - pi, pi);

            pi = prefix[pi - 1];
        }

        if (ch == pattern->line[pi]) {
            pi++;
        }

        pos++;
    }

    if (pi == pattern->length) {
        printf("%d %d", pos - pi, pi);
    }
    printf("\n");

    free(prefix);
}

Pattern new_pattern(int length, FILE *fp) {
    Pattern pattern = {
        .line = "",
        .length = 0,
    };

    if (fgets(pattern.line, length, fp) == NULL) {
        return pattern;
    }

    pattern.length = strlen(pattern.line);

    if (pattern.line[pattern.length - 1] == '\n') {
        pattern.line[--pattern.length] = '\0';
    }

    return pattern;
}

int main(void) {
    FILE *input = fopen("in.txt", "rb");

    if (input == NULL) {
        return EXIT_FAILURE;
    }

    Pattern pattern = new_pattern(PATTERN_LENGTH, input);

    if (pattern.length == 0) {
        fclose(input);
        return EXIT_SUCCESS;
    }

    CycleBuffer cyclebuffer = new_cyclebuffer(BUFFER_LENGTH, input);

    if (cyclebuffer.buffer == NULL) {
        fclose(input);
        return EXIT_FAILURE;
    }

    kmp(&pattern, &cyclebuffer);

    free(cyclebuffer.buffer);
    fclose(input);

    return EXIT_SUCCESS;
}
