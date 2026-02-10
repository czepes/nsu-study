#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define N 1000000

void swap(char *string, int i, int j) {
    char tmp = string[i];
    string[i] = string[j];
    string[j] = tmp;
}

void minimize(char *string, int length, long long moves) {
    int l = 0;

    while (l < length && string[l] == '0') l++;

    for (int r = l + 1; r < length && moves > 0; r++) {
        if (string[r] == '1') continue;
        if (r - l > moves) {
            l = r - moves;
        }
        swap(string, l, r);
        moves -= (r - l);
        l++;
    }
}

int main(void) {
    int tests = 0;

    if (scanf("%d", &tests) != 1) {
        return EXIT_FAILURE; 
    }
 
    int length;
    long long moves;
    char string[N];

    while (tests--) {
        if (scanf("%d %lld", &length, &moves) != 2) {
            return EXIT_FAILURE;
        }

        fgetc(stdin);
        if (fgets(string, length + 2, stdin) == NULL) {
            return EXIT_FAILURE;
        }
        string[length] = '\0';
        
        minimize(string, length, moves);

        printf("%s\n", string);
    }
 
    return EXIT_SUCCESS;
}
