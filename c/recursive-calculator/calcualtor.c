#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#define INPUT_FAILURE -1
#define CALC_SUCCESS 0
#define SYNTAX_ERROR 1
#define DIVISION_BY_ZERO 2
#define DEFAULT_CAPACITY 10

#define PLUS '+'
#define MINUS '-'
#define MUL '*'
#define DIV '/'
#define OPEN_BR '('
#define CLOSE_BR ')'

#define PLUS_PRIORITY 0
#define MINUS_PRIORITY 1
#define MUL_PRIORITY 2
#define DEFAULT_PRIORITY 3

int input_string(char **str_ptr, int capacity) {
    char *string = *str_ptr;
    char ch;
    int length = 0;

    while (scanf("%c", &ch) == 1) {
        if (string == NULL) {
            return INPUT_FAILURE;
        }
        if (ch == '\n') break;
        string[length++] = ch;
        if (length == capacity) {
            capacity *= 2;
            *str_ptr = realloc(string, capacity * sizeof(char));
            if (*str_ptr == NULL) {
                return INPUT_FAILURE;
            }
            string = *str_ptr;
        }
    }
    string[length] = '\0';

    *str_ptr = realloc(string, (length + 1) * sizeof(char));

    if (*str_ptr == NULL) {
        return INPUT_FAILURE;
    }

    return length;
}

int count_brackets(char *expr, int left, int right, int *exit_point) {
    int brackets = 0;

    for (int i = left; i <= right; i++) {

        if (expr[i] == OPEN_BR) brackets++;
        else if (expr[i] == CLOSE_BR) brackets--;

        if (brackets == 0) {
            *exit_point = i;
            return brackets;
        }
    }

    *exit_point = right;
    return brackets;
}

int remove_outer_brackets(char *expr, int left, int right) {
    int removed = 0;
    int exit_point = 0;

    while (expr[left] == OPEN_BR && expr[right] == CLOSE_BR) {

        int brackets = count_brackets(
            expr,
            left,
            right - 1,
            &exit_point
        );

        if (brackets == 0) {
            break;
        }

        left++;
        right--;
        removed++;
    }

    return removed;
}

int skip_inner_brackets(char *expr, int left, int right, int *status) {
    int exit_point;

    int brackets = count_brackets(expr, left, right, &exit_point);

    if (brackets != 0 || left + 1 == exit_point) {
        *status = SYNTAX_ERROR;
        return left;
    } 

    return exit_point;
}

bool is_op(char ch) {
    return ch == PLUS || ch == MINUS
        || ch == MUL || ch == DIV;
}

int op_priority(char op) {
    switch(op) {
        case(PLUS): return PLUS_PRIORITY;
        case(MINUS): return MINUS_PRIORITY;
        case(MUL): return MUL_PRIORITY;
        case(DIV): return MUL_PRIORITY;
    }
    return DEFAULT_PRIORITY;
}


int do_op(char op, int a, int b, int* status) {
    switch (op) {
        case PLUS: return a + b;
        case MINUS: return a - b;
        case MUL: return a * b;
        case DIV: {
            if (b == 0) {
                *status = DIVISION_BY_ZERO;
                return EXIT_FAILURE;
            }
            return a / b;
        }
    }
    *status = SYNTAX_ERROR;
    return EXIT_FAILURE;
}

int to_num(char *expr, int left, int right) {
    int num = 0;
    int order = 1;

    for (int i = right; i >= left; i--) {
        num += order * (expr[i] - '0');
        order *= 10;
    }

    return num;
}

int calc(char *expr, int left, int right, int *status) {
    if (*status != CALC_SUCCESS) {
        return EXIT_FAILURE;
    }

    // remove outer brackets
    int removed = remove_outer_brackets(expr, left, right);
    left += removed;
    right -= removed;

    if (left > right) {
        *status = SYNTAX_ERROR;
        return EXIT_FAILURE;
    }

    int op_index = -1;
    int saved_op_index = -1;
    int saved_priority = DEFAULT_PRIORITY;

    for (int i = left; i <= right; i++) {
        char ch = expr[i];

        if (isdigit(ch)) { // skip numbers
            continue;
        } else if (ch == OPEN_BR) { // skip brackets
            i = skip_inner_brackets(expr, i, right, status);
            if (*status != CALC_SUCCESS) return EXIT_FAILURE;
            continue;
        } else if (is_op(ch)) {
            op_index = i; // save operator
        } else {
            // wrong character
            *status = SYNTAX_ERROR;
            return EXIT_FAILURE;
        }

        if (op_index == left && expr[op_index] != MINUS) {
            *status = SYNTAX_ERROR;
            return EXIT_FAILURE;
        }

        if (
            op_index >= left && 
            op_priority(expr[op_index]) <= saved_priority
        ) {
            saved_op_index = op_index;
            saved_priority = op_priority(expr[saved_op_index]);
        }
    }

    op_index = saved_op_index;

    char operator = PLUS;
    int left_res = 0, right_res = 0;

    if (op_index >= left) {
        operator = expr[op_index];

        // handle negative numbers
        if (op_index != left || operator != MINUS) {
            left_res = calc(expr, left, op_index - 1, status);
        }

        right_res = calc(expr, op_index + 1, right, status);
    } else {
        left_res = to_num(expr, left, right);
    }

    return do_op(
        operator,
        left_res,
        right_res,
        status
    );
}

int main(void) {
    int capacity = DEFAULT_CAPACITY;
    char *expression = malloc(DEFAULT_CAPACITY * sizeof(char));
    int length = input_string(&expression, capacity);

    if (length == INPUT_FAILURE) {
        free(expression);
        return EXIT_FAILURE;
    }

    int status = CALC_SUCCESS;
    int result = calc(expression, 0, length - 1, &status);
    
    switch (status) {
        case CALC_SUCCESS: {
            printf("%d\n", result);
            break;
        }
        case SYNTAX_ERROR: {
            printf("syntax error\n");
            break;
        }
        case DIVISION_BY_ZERO: {
            printf("division by zero\n");
            break;
        }
    }

    free(expression);
    return EXIT_SUCCESS;
}
