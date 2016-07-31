#ifndef MATH_H
#define MATH_H

#include <array>

#include <unsupported/Eigen/CXX11/Tensor>

typedef float Scalar;
typedef Eigen::Tensor<Scalar, 2> Grid;
typedef Eigen::Array<Scalar, 2, 1> Location;
typedef Eigen::Array<Grid::Index, 2, 1> Indices;

// Bilinearly interpolates grid with x and y in i and j
Scalar interpolate(const Grid &grid, Scalar x, Scalar y);

#endif // MATH_H
