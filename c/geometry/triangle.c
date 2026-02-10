#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct point {
    int x;
    int y;
} Point;

typedef struct triangle {
    Point points[3];
} Triangle;

int determinant2(Point a, Point b, Point c) {
    // |ab x ac| = 
    // | ab.x ac.x | = ab.x * ac.y
    // | ab.y ac.y |   - ab.y * ac.x
    return \
        (b.x - a.x) * (c.y - a.y)
        - (b.y - a.y) * (c.x - a.x);
}

bool inTriangle(Point triangle[3], Point point) {

    int d[3];

    for (int i = 0; i < 3; i++) {

        d[i] = determinant2(
            point,
            triangle[i],
            triangle[(i + 1) % 3]
        );
    }

    return d[0] <= 0 && d[1] <= 0 && d[2] <= 0
        || d[0] >= 0 && d[1] >= 0 && d[2] >= 0;
}

int main() {
    Point points[4];

    for (int i = 0; i < 4; i++) {

        int x, y;

        if (scanf("%d %d", &x, &y) != 2) {
            return EXIT_FAILURE;
        }

        points[i] = (Point) {
            .x = x,
            .y = y
        };
    }

    printf("%s\n", inTriangle(points, points[3]) ? "In" : "Out");

    return EXIT_SUCCESS;
}
