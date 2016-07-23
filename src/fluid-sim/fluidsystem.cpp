#include "fluidsystem.h"

FluidSystem::FluidSystem(Scalar diffusionConstant, Scalar viscosity) :
    diffusionConstant(diffusionConstant), viscosity(viscosity)
{

}

void FluidSystem::clear() {
    this->u = Grid::Zero();
    this->v = Grid::Zero();
    this->u_prev = Grid::Zero();
    this->v_prev = Grid::Zero();
    this->density = Grid::Zero();
    this->density_prev = Grid::Zero();
}
