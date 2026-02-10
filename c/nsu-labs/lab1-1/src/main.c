#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cyclebuffer.h"
#include "hash.h"

#define PATTERN_LENGTH 16 + 2
#define BUFFER_SIZE 1024

typedef struct {
    char line[PATTERN_LENGTH];
    int length;
} Pattern;

Pattern new_pattern(FILE *fp) {
    Pattern pattern = {
        .length = 0,
        .line = ""
    };
    
    if (fgets(pattern.line, sizeof(pattern.line), fp) == NULL) {
        return pattern;
    }

    pattern.length = strlen(pattern.line);

    if (pattern.line[pattern.length - 1] == '\n') {
        pattern.line[--pattern.length] = '\0';
    }

    return pattern;
}

void match(Pattern *pattern, CycleBuffer *cb, int pos) {
    if (cb->size == 0) return;

    for (int i = 0; i < pattern->length; i++) {
        printf("%d ", pos + i);

        if (pattern->line[i] != read(cb, i)) break;
    }
}

void rabin_karp(Pattern *pattern, CycleBuffer *cb) {
    if (cb->size == 0) write(cb);

    int pattern_hash = poly_hash(pattern->line, pattern->length);

    printf("%d ", pattern_hash);
    
    int frame_hash = poly_hash(cb->buffer, pattern->length);
    int pw = get_pw(pattern->length);

    int pos = 1;

    while (cb->size >= pattern->length) {

        if (pattern_hash == frame_hash) {
            match(pattern, cb, pos);
        }

        char prev = extract(cb);
        char next = read(cb, pattern->length - 1);

        frame_hash = next_hash(
            frame_hash, pw,
            prev, next
        );

        if (cb->size * 2 <= cb->capacity) {
            write(cb);
        }

        pos++;
    }
    printf("\n");
}

int main(void) {
    FILE *input = fopen("in.txt", "rb");

    if (input == NULL) {
        printf("Failed to open file\n");
        return EXIT_FAILURE;
    }

    Pattern pattern = new_pattern(input);

    if (pattern.length == 0) {
        fclose(input);
        return EXIT_SUCCESS;
    }

    CycleBuffer cyclebuffer = new_cyclebuffer(BUFFER_SIZE, input);

    if (cyclebuffer.buffer == NULL) {
        fclose(input);
        return EXIT_FAILURE;
    }

    rabin_karp(&pattern, &cyclebuffer);

    fclose(input);
    free(cyclebuffer.buffer);

    return EXIT_SUCCESS;
}
