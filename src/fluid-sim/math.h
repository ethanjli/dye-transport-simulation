#ifndef MATH_H
#define MATH_H

#include <array>

#include <Eigen/Dense>

typedef float Scalar;
typedef Eigen::Array<Scalar, Eigen::Dynamic, Eigen::Dynamic> Grid;
typedef Eigen::Array<Scalar, 2, 1> Location;
typedef Eigen::Array<Grid::Index, 2, 1> Indices;

Scalar interpolate(const Grid &grid, Scalar x, Scalar y);

#endif // MATH_H
