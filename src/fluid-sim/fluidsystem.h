#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include <Eigen/Dense>

#include "math.h"

// Compile-time parameters
const unsigned int kGridSize = 100;
const double kGridLength = 1;

const unsigned int kFullGridSize = kGridSize + 2;
const double kGridSpacing = kGridLength / kGridSize;
typedef Eigen::Array<Scalar, kFullGridSize, kFullGridSize> Grid;

class FluidSystem
{
public:
    FluidSystem();

    Scalar diffusionConstant;
    Scalar viscosity;

    Grid u;
    Grid v;
    Grid u_prev;
    Grid v_prev;

    Grid density;
    Grid density_prev;
};

#endif // FLUIDSYSTEM_H
