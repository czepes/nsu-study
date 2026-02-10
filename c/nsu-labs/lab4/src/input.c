#include "input.h"

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
