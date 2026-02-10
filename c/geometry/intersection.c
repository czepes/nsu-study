#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct point {
    int x;
    int y;
} Point;

typedef struct segment {
    Point left;
    Point right;
} Segment;

void swap(int *a, int *b) {
    *a = *a ^ *b;
    *b = *a ^ *b;
    *a = *a ^ *b;
}

int determinant2(Point a, Point b, Point c) {
    return \
        (b.x - a.x) * (c.y - a.y)
        - (b.y - a.y) * (c.x - a.x);
}

bool onSegment(Segment s, Point p) {
    return s.left.x <= p.x && p.x <= s.right.x && (
        s.left.y <= p.y && p.y <= s.right.y
        || s.right.y <= p.y && p.y <= s.left.y
    );
}

bool intersects(Segment s1, Segment s2) {

    int d1 = determinant2(s1.left, s1.right, s2.left);
    int d2 = determinant2(s1.left, s1.right, s2.right);
    int d3 = determinant2(s2.left, s2.right, s1.left);
    int d4 = determinant2(s2.left, s2.right, s1.right);

    if (((d1 < 0 && d2 > 0) || (d2 < 0 && d1 > 0)) && 
        ((d3 < 0 && d4 > 0) || (d4 < 0 && d3 > 0))) {
        return true;
    }

    if (d1 == 0 && onSegment(s1, s2.left)) return true;
    if (d2 == 0 && onSegment(s1, s2.right)) return true;
    if (d3 == 0 && onSegment(s2, s1.left)) return true;
    if (d4 == 0 && onSegment(s2, s1.right)) return true;

    return false;
}

int main() {
    Segment segments[2];

    for (int i = 0; i < 2; i++) {

        int xl, yl, xr, yr;

        if (scanf("%d %d %d %d", &xl, &yl, &xr, &yr) != 4) {
            return EXIT_FAILURE;
        }

        if (xr < xl) {
            swap(&xl, &xr);
            swap(&yl, &yr);
        }

        segments[i].left = (Point) {
            .x = xl,
            .y = yl
        };
        segments[i].right = (Point) {
            .x = xr,
            .y = yr
        };
    }

    bool result = intersects(segments[0], segments[1]);
    printf("%s\n", result ? "Yes" : "No");

    return EXIT_SUCCESS;
}
