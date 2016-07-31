#include "math.h"

Scalar interpolate(const Grid &grid, Location x) {
    Indices i = x.cast<Grid::Index>();
    Indices j = i + 1;
    Location t = x - i.cast<Scalar>();
    Location s = 1 - t;
    return (s[1] * (s[0] * grid(i[0], i[1], 0) +
                    t[0] * grid(j[0], i[1], 0)) +
            t[1] * (s[0] * grid(i[0], j[1], 0) +
                    t[0] * grid(j[0], j[1], 0)));
}
