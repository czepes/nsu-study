#include <stdlib.h>
#include <stdio.h>

typedef struct point {
    int x;
    int y;
} Point;

int determinant2(Point a, Point b, Point c) {
    return \
        (b.x - a.x) * (c.y - a.y)
        - (b.y - a.y) * (c.x - a.x);
}

double calcArea(int size, Point figure[size]) {

    int area = 0;

    for (int i = 2; i < size; i++) {
        area += determinant2(
            figure[0],
            figure[i - 1],
            figure[i]
        );
    }

    return ((double) abs(area)) / 2;
}

int main() {

    int size;

    if (scanf("%d", &size) != 1) {
        return EXIT_FAILURE;
    }

    Point figure[size];

    for (int i = 0; i < size; i++) {
        
        int x, y;

        if (scanf("%d %d", &x, &y) != 2) {
            return EXIT_FAILURE;
        }

        figure[i] = (Point) {
            .x = x,
            .y = y
        };
    }

    printf("%.1f\n", calcArea(size, figure));

    return EXIT_SUCCESS;
}
