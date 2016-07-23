#include "fluidsystem.h"

FluidSystem::FluidSystem(Scalar diffusionConstant, Scalar viscosity) :
    diffusionConstant(diffusionConstant), viscosity(viscosity)
{
}

void FluidSystem::clear() {
    this->u = Grid::Zero(kFullGridSize, kFullGridSize);
    this->v = Grid::Zero(kFullGridSize, kFullGridSize);
    this->u_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    this->v_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    this->density = Grid::Zero(kFullGridSize, kFullGridSize);
    this->density_prev = Grid::Zero(kFullGridSize, kFullGridSize);
}
