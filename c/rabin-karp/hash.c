#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define P 3
#define MAX_LENGTH 16 + 2 // +2 for \n and \0

int calculate_hash(const char* str, int length) { 
    int hash = 0;
    int power = 1;

    for (int i = 0; i < length; i++) {
        hash += ((unsigned char) str[i] % P) * power;
        power *= P;
    }

    return hash;
}

int main() {
    FILE *input = fopen("patterns.txt", "rb");

    int length = MAX_LENGTH;
    char pattern[length];

    while (fgets(pattern, length, input) != NULL) {
        length = strlen(pattern);

        if (pattern[length - 1] == '\n') {
            pattern[--length] = '\0';
        }

        printf("%s: %d\n", pattern, calculate_hash(pattern, length));
    }

    fclose(input);
    return EXIT_SUCCESS;
}
