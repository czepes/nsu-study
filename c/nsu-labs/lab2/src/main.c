#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define INPUT_FAILURE -1
#define NO_SUFFIX -1
#define DEFAULT_LENGTH 10

bool check_permutation(char *permutation) {
    int length = strlen(permutation);
    bool visited['9' - '0' + 1] = {false};

    for (int i = 0; i < length; i++) {
        char ch = permutation[i];
        if (ch < '0' || ch > '9') {
            return false;
        }
        if (visited[ch - '0']) {
            return false;
        }
        visited[ch - '0'] = true;
    }

    return true;
}

void swap(char *string, int i, int j) {
    char tmp = string[i];
    string[i] = string[j];
    string[j] = tmp;
}

void reverse(char *string, int start, int end) {
    while (start < end) {
        swap(string, start++, end--);
    }
}

int get_permutation_suffix(char *permutation, int length) {
    int suffix = NO_SUFFIX;

    for (int i = length - 1; i > 0; i--) {
        if (permutation[i - 1] < permutation[i]) {
            suffix = i;
            break;
        }
    }

    return suffix;
}

int get_suffix_upper_bound(char *permutation, int length, int suffix) {
    int index = suffix;

    for (int i = suffix + 1; i < length; i++) {
        if (permutation[suffix - 1] < permutation[i]) {
            if (permutation[i] < permutation[index]) {
                index = i;
            }
        }
    }

    return index;
}

bool next_permutation(char *permutation, int length) {
    int suffix = get_permutation_suffix(permutation, length);
    if (suffix == NO_SUFFIX) {
        return false;
    }

    int swap_index = get_suffix_upper_bound(permutation, length, suffix);

    swap(permutation, suffix - 1, swap_index);
    reverse(permutation, suffix, length - 1);

    return true;
}

int permutation_input(char *permutation, int length) {
    if (fgets(permutation, length, stdin) == NULL) {
        return INPUT_FAILURE;
    }

    length = strlen(permutation);

    if (permutation[length - 1] == '\n') {
        permutation[--length] = '\0';
    }

    if (length > DEFAULT_LENGTH) {
        return INPUT_FAILURE;
    }

    if (!check_permutation(permutation)) {
        return INPUT_FAILURE;
    }

    return length;
}

int main(void) {
    int length = DEFAULT_LENGTH + 3; // +2 for '\n', '\0' and +1 to check DEFAULT_LENGTH
    char permutation[length];

    length = permutation_input(permutation, length);

    if (length == INPUT_FAILURE) {
        printf("bad input\n");
        return EXIT_SUCCESS;
    }

    int amount;

    if (scanf("%d", &amount) != 1) {
        return EXIT_FAILURE;
    }
    
    for (int i = 0; i < amount; i++) {
        if (!next_permutation(permutation, length)) {
            break;
        }
        printf("%s\n", permutation);
    }

    return EXIT_SUCCESS;
}
