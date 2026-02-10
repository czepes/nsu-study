#ifndef GEOMETRY_HEADER
#define GEOMETRY_HEADER

#include <stdlib.h>
#include "stack.h"

typedef struct point {
    int x;
    int y;
} Point;

long long determinant2(Point *a, Point *b, Point *c);

#endif
