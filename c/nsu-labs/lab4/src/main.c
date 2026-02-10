#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "stack.h"
#include "input.h"

#define CALC_SUCCESS 0
#define CALC_FAILURE 1

#define SYNTAX_ERROR 2
#define DIVISION_BY_ZERO 3
#define MEMORY_ERROR 4

#define DEFAULT_CAPACITY 10

#define PLUS '+'
#define MINUS '-'
#define MUL '*'
#define DIV '/'
#define OPEN_BR '('
#define CLOSE_BR ')'
#define EQUALS '='

#define ADD_PRIORITY 1
#define MUL_PRIORITY 2
#define BR_PRIORITY 3
#define DEFAULT_PRIORITY 0

bool is_op(char ch) {
    return ch == PLUS
        || ch == MINUS
        || ch == MUL
        || ch == DIV;
}

bool in_expr(char ch) {
    return is_op(ch)
        || ch == OPEN_BR
        || ch == CLOSE_BR
        || ch == EQUALS;
}

bool is_op_legit(char prev, char next) {
    return !is_op(prev) && prev != OPEN_BR
        && !is_op(next) && next != CLOSE_BR;
}

int get_priority(char op, int depth) {

    int result = depth * BR_PRIORITY;

    switch(op) {
        case(PLUS):
        case(MINUS): {
            result += ADD_PRIORITY;
            break;
        }
        case(MUL):
        case(DIV): {
            result += MUL_PRIORITY;
            break;
        }
        case(OPEN_BR):
        case(CLOSE_BR): {
            result += BR_PRIORITY;
            break;
        }
        default: result = DEFAULT_PRIORITY;
    }

    return result;
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

bool is_expr(char *expr, int length) {

    if (length <= 0) return false;

    int depth = 0;

    for (int i = 0; i < length; i++) {
        char cur = expr[i];

        if (depth < 0) return false;

        char prev = i > 0 ? expr[i - 1] : OPEN_BR;
        char next = i < length - 1 ? expr[i + 1] : CLOSE_BR;

        if (isdigit(cur)) continue;
        if (!in_expr(cur)) return false;

        switch (cur) {
            case EQUALS: return false;
            case OPEN_BR: {
                depth++;
                continue;
            }
            case CLOSE_BR: {
                if (prev == OPEN_BR) return false;
                depth--;
                continue;
            }
        }

        if (is_op(cur) && !is_op_legit(prev, next)) {
            return false;
        }

    }

    return depth == 0;
}

char* append_equal_sign(char *expr) {

    char equal[] = "=";
    char *source = malloc((strlen(expr) + sizeof(equal)) * sizeof(char));

    if (source != NULL) {
        source = strcpy(source, expr);
        source = strcat(source, equal);
    }

    return source;
}

int calculate(char *expr, int length, int *status) {
    
    if (!is_expr(expr, length)) {
        *status = SYNTAX_ERROR;
        return EXIT_FAILURE;
    }

    expr = append_equal_sign(expr);

    if (expr == NULL) {
        *status = MEMORY_ERROR;
        return EXIT_FAILURE;
    }

    length = strlen(expr);

    int num_stack[length];
    int num_top = -1;
    int depth = 0;
    int number_start = -1;

    Stack *op_stack = new_stack();

    for (int i = 0; i < length; i++) {

        char cur = expr[i];

        if (*status != CALC_SUCCESS) return CALC_FAILURE;

        if (isdigit(cur)) {
            if (number_start < 0) number_start = i;
            continue;
        }

        if (number_start >= 0) {
            num_stack[++num_top] = to_num(expr, number_start, i - 1);
            number_start = -1;
        }

        if (cur == CLOSE_BR) depth--;

        while (
            *status == CALC_SUCCESS &&
            op_stack->top != NULL &&
            get_priority(cur, depth) <= op_stack->top->priority
        ) {
            char op = op_stack->top->op;

            int a = num_stack[num_top--];
            int b = num_stack[num_top];

            num_stack[num_top] = do_op(op, b, a, status);

            rm(op_stack);
        }

        if (cur == OPEN_BR) depth++;

        if (is_op(cur)) {
            push(op_stack, cur, get_priority(cur, depth));
        }
    }

    clean(op_stack);

    free(expr);
    return num_stack[num_top];
}

int main(void) {
    int capacity = DEFAULT_CAPACITY;
    char *expr = malloc(DEFAULT_CAPACITY * sizeof(char));

    if (expr == NULL) {
        return EXIT_FAILURE;
    }

    int length = input_str(&expr, capacity);

    if (length == INPUT_FAILURE) {
        free(expr);
        return EXIT_FAILURE;
    }

    int status = CALC_SUCCESS;
    int result = calculate(expr, length, &status);

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
        case MEMORY_ERROR: {
            printf("memory error\n");
            break;
        }
    }

    free(expr);
    return EXIT_SUCCESS;
}
