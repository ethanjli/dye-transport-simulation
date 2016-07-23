#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include <Eigen/Dense>

#include "math.h"

// Compile-time parameters
const unsigned int kGridSize = 64;
const double kGridLength = 1;

const unsigned int kFullGridSize = kGridSize + 2;
const double kGridSpacing = kGridLength / kGridSize;
typedef Eigen::Array<Scalar, kFullGridSize, kFullGridSize> Grid;

class FluidSystem
{
public:
    FluidSystem(Scalar diffusionConstant = 0, Scalar viscosity = 0);

    Scalar diffusionConstant;
    Scalar viscosity;

    Grid u;
    Grid v;
    Grid u_prev;
    Grid v_prev;
    Grid u_add;
    Grid v_add;

    Grid density;
    Grid density_prev;
    Grid density_add;

    void clear();
};

#endif // FLUIDSYSTEM_H
