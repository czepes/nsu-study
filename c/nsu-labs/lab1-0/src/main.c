#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "stoptable.h"
#include "cyclebuffer.h"

#define INPUT_FILE "in.txt"

#define MAX_PATTERN_LENGTH 16 + 2
#define BUFFER_SIZE 1024

typedef struct {
    char line[MAX_PATTERN_LENGTH]; // +2 for '\n' and '\0'
    int length;
} Pattern;

char match(Pattern *pattern, CycleBuffer *cb, int pos) {
    for (int i = 0; i < pattern->length; i++) {
        printf("%d ", pos - i);

        int index = pattern->length - i - 1;

        if (pattern->line[index] != read(cb, index)) {
            break;
        }
    }

    return read(cb, pattern->length - 1);
}

void boyer_moore(Pattern *pattern, StopTable *st, CycleBuffer *cb) {
    if (cb->size == 0) {
        write(cb);
    }

    for (
        int pos = pattern->length, shift = 0;
        cb->size >= pattern->length;
        pos += shift
    ) {
        char last = match(pattern, cb, pos);
        shift = get_stop_value(st, last);

        unwrite(cb, shift);

        if (cb->size * 2 <= cb->capacity) {
            write(cb);
        }
    }
    printf("\n");
}

Pattern new_pattern(FILE *fp) {
    Pattern pattern = {
        .line = "",
        .length = 0,
    };

    // read pattern
    if (fgets(pattern.line, sizeof(pattern.line), fp) == NULL) {
        return pattern;
    }

    pattern.length = strlen(pattern.line);

    // remove '\n' at the end of the line
    if (pattern.line[pattern.length - 1] == '\n') {
        pattern.line[--pattern.length] = '\0';
    }

    return pattern;
}

int main(void) {
    FILE *input = fopen(INPUT_FILE, "rb");

    if (input == NULL) {
        printf("Error at opening file\n");
        return EXIT_FAILURE;
    }

    Pattern pattern = new_pattern(input);

    if (pattern.length == 0) {
        fclose(input);
        return EXIT_SUCCESS;
    }

    // create table of stop symbols and text reader
    StopTable stoptable = new_stoptable(pattern.line, pattern.length);
    CycleBuffer cyclebuffer = new_cyclebuffer(BUFFER_SIZE, input);

    // Boyer Moore algorithm
    boyer_moore(&pattern, &stoptable, &cyclebuffer);

    free(cyclebuffer.buffer);
    fclose(input);

    return EXIT_SUCCESS;
}
