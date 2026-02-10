#include "geometry.h"

long long determinant2(Point *a, Point *b, Point *c) {
    return \
        ((long long) b->x - a->x) * ((long long) c->y - a->y)
        - ((long long) b->y - a->y) * ((long long) c->x - a->x);
}

