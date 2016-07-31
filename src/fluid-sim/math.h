#ifndef MATH_H
#define MATH_H

#include <array>

#include <unsupported/Eigen/CXX11/Tensor>

typedef float Scalar;
const Eigen::Index kGridDimensions = 3;
typedef Eigen::Tensor<Scalar, kGridDimensions> Grid;
typedef Eigen::Array<Scalar, kGridDimensions, 1> Location;
typedef Eigen::Array<Grid::Index, kGridDimensions, 1> Indices;

// Linearly interpolates grid to nearest neighbors
Scalar interpolate(const Grid &grid, Location x);

#endif // MATH_H
