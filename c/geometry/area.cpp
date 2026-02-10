#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

struct Point {
    int x;
    int y;
};

int determinant2(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

double calcArea(const std::vector<Point>& figure) {
    double area = 0.0;
    size_t size = figure.size();

    for (size_t i = 2; i < size; ++i) {
        area += determinant2(figure[0], figure[i - 1], figure[i]);
    }

    area = std::abs(area);
    return area / 2.0;
}

int main() {
    int size;

    if (!(std::cin >> size) || size < 3) {
        return EXIT_FAILURE;
    }

    std::vector<Point> figure(size);

    for (int i = 0; i < size; ++i) {
        if (!(std::cin >> figure[i].x >> figure[i].y)) {
            return EXIT_FAILURE;
        }
    }

    double area = calcArea(figure);
    std::cout << "" << std::fixed << std::setprecision(1) << area << std::endl;

    return EXIT_SUCCESS;
}
