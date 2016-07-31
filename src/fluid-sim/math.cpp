#include "math.h"

Scalar interpolate(const Grid &grid, Scalar x, Scalar y) {
    Location X = {x, y};
    Indices i = X.cast<Grid::Index>();
    Indices j = i + 1;
    Location t = X - i.cast<Scalar>();
    Location s = 1 - t;
    return (s[0] * (s[1] * grid(i[0], i[1], 0) + t[1] * grid(i[0], j[1], 0)) +
            t[0] * (s[1] * grid(j[0], i[1], 0) + t[1] * grid(j[0], j[1], 0)));
}
