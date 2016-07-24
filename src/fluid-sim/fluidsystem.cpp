#include "fluidsystem.h"

FluidSystem::FluidSystem(Scalar diffusionConstant, Scalar viscosity) :
    diffusionConstant(diffusionConstant), viscosity(viscosity)
{
}

void FluidSystem::clear() {
    u = Grid::Zero(kFullGridSize, kFullGridSize);
    v = Grid::Zero(kFullGridSize, kFullGridSize);
    u_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    v_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    density = Grid::Zero(kFullGridSize, kFullGridSize);
    density_prev = Grid::Zero(kFullGridSize, kFullGridSize);
}
