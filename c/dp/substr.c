#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define INPUT_FAILURE -1
#define MEMORY_FAILURE -2
#define DEFAULT_LENGTH 16

int input_str(char **str_ptr, int capacity) {

    char *str = *str_ptr;
    char ch;
    int length = 0;

    while (scanf("%c", &ch) == 1) {

        if (str == NULL) {
            return INPUT_FAILURE;
        }

        if (ch == '\n') break;

        str[length++] = ch;

        if (length == capacity) {

            capacity *= 2;
            *str_ptr = realloc(str, capacity * sizeof(char));

            if (*str_ptr == NULL) {
                return INPUT_FAILURE;
            }

            str = *str_ptr;
        }
    }
    str[length] = '\0';

    *str_ptr = realloc(str, (length + 1) * sizeof(char));

    if (*str_ptr == NULL) {
        return INPUT_FAILURE;
    }

    return length;
}

int count_substr(char *str, int length) {

    int *seen = calloc(sizeof(int), UCHAR_MAX + 1);

    if (seen == NULL) return MEMORY_FAILURE;

    int count = 0;
    int substr_len = 0;
    int from = 0;

    for (int i = 0; i < length; i++) {
        char ch = str[i];

        if (seen[ch] < from) substr_len++;
        else {
            substr_len = i - seen[ch] + 1;
            from = seen[ch];
        }

        seen[ch] = i + 1;
        count += substr_len;
    }
    
    free(seen);
    return count;
}

int main(void) {

    int length = DEFAULT_LENGTH;
    char *str = malloc(length * sizeof(char));

    length = input_str(&str, length);

    int result = count_substr(str, length);

    if (result == MEMORY_FAILURE) {
        printf("Failed to allocate memory\n");
    } else {
        printf("%d\n", result);
    }

    free(str);
    return EXIT_SUCCESS;
}
