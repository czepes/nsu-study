#include <stdlib.h>
#include <stdio.h>
#include "geometry.h"
#include "sort.h"
#include "stack.h"

typedef enum {
    NEGATIVE = -1,
    POSITIVE = +1
} Sign;

Point pool[N];

Point* getPoint(int id) {
    return &pool[id];
}

void setPoint(int id, int x, int y) {
    pool[id].x = x;
    pool[id].y = y;
}

long long absolute(long long value) {
    return value < 0 ? -value : value; 
}

Sign signOf(long long value) {
    return value < 0 ? NEGATIVE : POSITIVE;
}

CompareResult compare(int *points, int i, int j) {
    Point *o = getPoint(points[0]);
    Point *a = getPoint(points[i]);
    Point *b = getPoint(points[j]);

    long long dx1 = ((long long) a->x - o->x);
    long long dy1 = ((long long) a->y - o->y);
    long long dx2 = ((long long) b->x - o->x);
    long long dy2 = ((long long) b->y - o->y);

    Sign leftSign = signOf(dy1) * signOf(dx2);
    Sign rightSign = signOf(dy2) * signOf(dx1);

    if (leftSign != rightSign) {
        return leftSign > rightSign ? MORE : LESS;
    }

    unsigned long long dLeft = (unsigned long long) dy1 * dx2;
    unsigned long long dRight = (unsigned long long) dy2 * dx1;

    if (dLeft > dRight) {
        return MORE;
    }
    if (dLeft < dRight) {
        return LESS;
    } 

    long long dist1 = absolute(dx1) + absolute(dy1);
    long long dist2 = absolute(dx2) + absolute(dy2);

    if (dist1 < dist2) return LESS;
    if (dist1 > dist2) return MORE;

    return EQUAL;
}

void sortPoints(int size, int points[size]) {
    if (size <= 1) return;

    int leftIdx = 0;

    for (int i = 0; i < size; i++) {
        Point *point = getPoint(points[i]);
        Point *left = getPoint(points[leftIdx]);

        if (
            point->x < left->x ||
            (point->x == left->x && point->y < left->y)
        ) {
            leftIdx = i;
        }
    }

    swap(points, points + leftIdx);
    quicksort(points, 1, size - 1, compare);
}

void buildShell(int size, int points[size], Stack *shell) {
    if (size == 0 || points == NULL || shell == NULL) return;

    if (size == 1) {
        push(shell, points[0]);
        return;
    }

    push(shell, points[0]);
    push(shell, points[1]);

    for (int i = 2; i < size; i++) {
        Point *last2 = getPoint(shell->content[shell->size - 2]);
        Point *last1 = getPoint(shell->content[shell->size - 1]);
        Point *current = getPoint(points[i]);

        long long d = determinant2(last1, current, last2);

        if (d >= 0) {
            if (d == 0) pop(shell);
            push(shell, points[i]);
        } else {
            pop(shell);
            i--;
        }
    }
}

int main() {
    int amount;

    if (scanf("%d", &amount) != 1) {
        return EXIT_FAILURE;
    }

    if (amount < 0 || N < amount) {
        printf("bad number of points\n");
        return EXIT_SUCCESS;
    }

    int points[amount];

    for (int i = 0; i < amount; i++) {
        int x, y;

        if (scanf("%d %d", &x, &y) != 2) {
            printf("bad number of lines\n");
            return EXIT_SUCCESS;
        }

        setPoint(i, x, y);
        points[i] = i;
    }

    sortPoints(amount, points);

    Stack shell = {
        .size = 0,
        .content = {0},
    };

    buildShell(amount, points, &shell);

    while (shell.size > 0) {
        Point *point = getPoint(pop(&shell));
        printf("%d %d\n", point->x, point->y);
    }

    return EXIT_SUCCESS;
}
