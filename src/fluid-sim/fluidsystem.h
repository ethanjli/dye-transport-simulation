#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include <Eigen/Dense>

#include "math.h"

// Compile-time parameters
const unsigned int kGridSize = 200;
const double kGridLength = 1;

const unsigned int kFullGridSize = kGridSize + 2;
const double kGridSpacing = kGridLength / kGridSize;
typedef Eigen::Array<Scalar, Eigen::Dynamic, Eigen::Dynamic> Grid;

class FluidSystem
{
public:
    FluidSystem(Scalar diffusionConstant = 0, Scalar viscosity = 0);

    Scalar diffusionConstant;
    Scalar viscosity;

    Grid u = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid v = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid u_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid v_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid u_add = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid v_add = Grid::Zero(kFullGridSize, kFullGridSize);

    Grid density = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid density_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid density_add = Grid::Zero(kFullGridSize, kFullGridSize);

    void clear();
};

#endif // FLUIDSYSTEM_H
