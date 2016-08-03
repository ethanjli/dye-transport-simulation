#ifndef MATH_H
#define MATH_H

#include <array>

#include <unsupported/Eigen/CXX11/Tensor>

typedef float Scalar;
const Eigen::Index kGridDimensions = 3;
typedef Eigen::Tensor<Scalar, kGridDimensions> Grid;
typedef Eigen::Array<Scalar, kGridDimensions, 1> Location;
typedef Eigen::Array<Grid::Index, kGridDimensions, 1> Indices;
typedef std::array<Grid::Index, kGridDimensions> TensorIndices;
typedef std::function<void(Grid&)> BoundarySetter;

void linearSolve(Grid &solution, const Grid &initial, Scalar alpha, Scalar beta,
                 const Indices &dim, BoundarySetter setBoundaries,
                 unsigned int iterations = 20);

// Linearly interpolates grid to nearest neighbors
Scalar interpolate(const Grid &grid, Location x);

// Sets boundary conditions on grids
void setBoundaries(Grid &grid, int b, const Indices &dim);
void setContinuityBoundaries(Grid &grid, const Indices &dim);
void setHorizontalNeumannBoundaries(Grid &grid, const Indices &dim);
void setVerticalNeumannBoundaries(Grid &grid, const Indices &dim);
void setDepthNeumannBoundaries(Grid &grid, const Indices &dim);

#endif // MATH_H
