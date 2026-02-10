#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define ERROR_MSG "bad input"

#define INPUT_ERROR -1
#define INVALID_CHAR -1

#define MIN_BASE 2
#define MAX_BASE 16
#define MAX_NUMBER_SIZE 13
#define MAX_FRACTION_SIZE 12
#define MAX_BUFFER_SIZE 64

char num_to_base(unsigned int value) {
    if (value > 15) return '\0';
    if (value >= 10 && value <= 15) {
        return value - 10 + 'a';
    }
    return value + '0';
}

int base_to_num(char ch) {
    if (ch >= 'A' && ch <= 'F') {
        return 10 + ch - 'A';
    } else if (ch >= 'a' && ch <= 'f') {
        return 10 + ch - 'a';
    } else if (isdigit(ch)) {
        return ch - '0';
    }
    return INVALID_CHAR;
}

void converse_from_base(
    char* base_num, int length, 
    int base, long long* int_ptr, double* frac_ptr
) {
    *int_ptr = 0;
    *frac_ptr = 0;

    int dot_idx = 0;

    while (
        dot_idx < length
        && base_num[dot_idx] != '.'
    ) dot_idx++;

    long long pw = 1;

    for (
        int i = dot_idx - 1, j = dot_idx + 1;
        i >= 0 || j < length;
        i--, j++
    ) {
        if (i >= 0) {
            *int_ptr += base_to_num(base_num[i]) * pw;
        }
        pw *= base;
        if (j < length) {
            *frac_ptr += base_to_num(base_num[j]) * (1 / (double) pw);
        }
    }
}

void swap(char* string, int i, int j) {
    char tmp = string[i];
    string[i] = string[j];
    string[j] = tmp;
}

char converse_to_base(
    char* base_num, int capacity,
    int base, long long int_part, double frac_part
) {
    int rem = 0;
    int length = 0;

    // convert integer part
    do {
        rem = int_part % base;
        int_part /= base;
        base_num[length++] = num_to_base(rem);
    } while (
        length < capacity - 2
        && int_part > 0
    );
    // reverse order -> normal order
    for (int i = length - 1, j = 0; i > j; i--, j++) {
        swap(base_num, i, j);
    }

    if (frac_part == 0) return length;
    base_num[length++] = '.';

    // convert fractional part
    int int_length = length;
    while (
        length < capacity - 1
        && length - int_length < MAX_FRACTION_SIZE
        && frac_part > 0
    ) {
        frac_part = frac_part * base;
        rem = (long long) frac_part;
        frac_part = frac_part - rem;
        base_num[length++] = num_to_base(rem);
    }

    base_num[length] = '\0';

    return length;
}

int input_base_num(char** base_ptr, int capacity, int base) {
    char* base_num = malloc(capacity * sizeof(char));

    if (base_num == NULL) {
        return INPUT_ERROR;
    }

    int length = 0;
    char ch;
    int dot_idx = -1;

    while (
        length < capacity - 1 
        && scanf("%c", &ch) == 1
    ) {
        if (ch == '\n') {
            break;
        }

        if (ch == '.') {
            // found second dot
            if (dot_idx >= 0) {
                free(base_num);
                return INPUT_ERROR;
            }
            dot_idx = length;
        } else {
            int digit = base_to_num(ch);

            // char is not a number or not in range
            if (digit < 0 || digit >= base) {
                free(base_num);
                return INPUT_ERROR;
            }
        }

        base_num[length++] = ch;
    }

    // special case for input = 0. or .0
    if (dot_idx == 0 || dot_idx == length - 1) {
        free(base_num);
        return INPUT_ERROR;
    }

    if (base_num[length - 1] == '\n') {
        length--;
    }
    base_num[length] = '\0';

    *base_ptr = base_num;
    return length;
}


int main(void) {
    int base_in = 0;
    int base_out = 0;

    if ((
        scanf("%d", &base_in) != 1
        || base_in < MIN_BASE
        || base_in > MAX_BASE
        ) || (
        scanf("%d", &base_out) != 1
        || base_out < MIN_BASE
        || base_out > MAX_BASE
    )) {
        printf("%s\n", ERROR_MSG);
        return EXIT_SUCCESS;
    }
    fgetc(stdin); // remove '\n' at the end of the line

    char* base_num = NULL;
    int length = input_base_num(
        &base_num,
        MAX_NUMBER_SIZE + 1,
        base_in
    );

    if (length <= 0) {
        printf("%s\n", ERROR_MSG);
        return EXIT_SUCCESS;
    }

    long long int_part = 0;
    double frac_part = 0;

    // base conversion to decimal
    converse_from_base(
        base_num, length,
        base_in, &int_part, &frac_part
    );

    free(base_num);
    length = MAX_BUFFER_SIZE;
    base_num = malloc(length * sizeof(char));

    if (base_num == NULL) {
        printf("failed to allocate memory\n");
        return EXIT_FAILURE;
    }

    // base conversion from decimal
    converse_to_base(
        base_num, length,
        base_out, int_part, frac_part
    );

    printf("%s\n", base_num);

    free(base_num);
    return EXIT_SUCCESS;
}
