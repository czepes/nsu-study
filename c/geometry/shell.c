#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define N 1000

#define LESS -1
#define MORE 1
#define EQUAL 0

typedef struct point {
    int x;
    int y;
} Point;

typedef struct stack {
    int size;
    Point content[N];
} Stack;

void push(Stack *stack, Point value) {
    stack->content[stack->size++] = value;
}

Point pop(Stack *stack) {
    if (stack->size == 0) {
        fprintf(stderr, "Pop from empty stack\n");
    }
    return stack->content[--stack->size];
}

void swap(Point *a, Point *b) {
    Point tmp = *a;
    *a = *b;
    *b = tmp;
}

int vectorLength(Point a, Point b) {

    int dy = b.y - a.y;
    int dx = b.x - a.x;

    return dy * dy + dx * dx;
}

int comp(Point o, Point a, Point b) {

    int dy1 = a.y - o.y;
    int dx1 = a.x - o.x;

    int dy2 = b.y - o.y;
    int dx2 = b.x - o.x;

    if (dx1 == 0) {
        if (dx2 != 0) return MORE;
        if (dy1 <= dy2) return LESS;
        else return MORE;
    }

    if (dx2 == 0) {
        if (dx1 != 0) return LESS;
        if (dy1 <= dy2) return MORE;
        else return LESS;
    }

    double tan1 = ((double) dy1) / dx1;
    double tan2 = ((double) dy2) / dx2;

    if (tan1 < tan2) return LESS;
    if (tan1 > tan2) return MORE;
    if (tan1 == tan2) {
        if (dx1 <= dx2) return LESS;
        else return MORE;
    }

    return EQUAL;
}

int partition(Point* arr, int left, int right) {
    if (left >= right) return left;

    int middle = (right + left) / 2;

    for (int i = left, j = right; i < j;) {
        while (i < middle && comp(arr[0], arr[i], arr[middle]) <= 0) {
            i++;
        }
        while (j > middle && comp(arr[0], arr[j], arr[middle]) >= 0) {
            j--;
        }

        if (i == middle) middle = j;
        else if (j == middle) middle = i;

        swap(arr + i, arr + j);
    }

    return middle;
}

void quicksort(Point* arr, int left, int right) {
    if (left >= right) return;

    int middle = partition(arr, left, right);

    quicksort(arr, left, middle - 1);
    quicksort(arr, middle + 1, right);
}

void sortPoints(int size, Point points[size]) {

    int leftIdx = 0;

    for (int i = 0; i < size; i++) {
        if (points[i].x < points[leftIdx].x) {
            leftIdx = i;
        }
    }

    for (int i = 0; i < size; i++) {
        if (points[i].x > points[leftIdx].x) continue;
        if (points[i].y < points[leftIdx].y) {
            leftIdx = i;
        }
    }

    swap(points, points + leftIdx);
    quicksort(points, 1, size - 1);
}

int determinant2(Point a, Point b, Point c) {
    return \
        (b.x - a.x) * (c.y - a.y)
        - (b.y - a.y) * (c.x - a.x);
}

void buildShell(int size, Point *points, Stack *shell) {

    push(shell, points[0]);
    push(shell, points[1]);

    for (int i = 2; i < size; i++) {
        
        Point last2 = shell->content[shell->size - 2];
        Point last1 = shell->content[shell->size - 1];
        Point current = points[i];

        int d = determinant2(last1, current, last2);

        if (d >= 0) {
            push(shell, current);
        } else {
            pop(shell);
            i--;
        }
    }
}

double calcPerimeter(int size, Point figure[size]) {

    double p;

    for (int i = 0; i < size; i++) {

        Point a = figure[i];
        Point b = figure[(i + 1) % size];

        double length = sqrt((double) vectorLength(a, b));
        p += length;
    }

    return p;
}

int main() {
    int size;

    if (scanf("%d", &size) != 1) {
        return EXIT_FAILURE;
    }

    Point points[size];

    for (int i = 0; i < size; i++) {

        int x, y;

        if (scanf("%d %d", &x, &y) != 2) {
            return EXIT_FAILURE;
        }

        points[i] = (Point) {
            .x = x,
            .y = y
        };
    }

    sortPoints(size, points);

    Stack shell = {
        .size = 0,
        .content = {0}
    };

    buildShell(size, points, &shell);

    printf("%.1f\n", calcPerimeter(shell.size, shell.content));

    return EXIT_SUCCESS;
}
