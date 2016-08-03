#include "fluidsystem.h"

#include <utility>
#include <algorithm>

FluidSystem::FluidSystem(Grid::Index width, Grid::Index height, Grid::Index depth,
                         Scalar diffusionConstant, Scalar viscosity) :
    dim({width, height, depth}), fullDim(dim + 2),
    fullTensorDim({width + 2, height + 2, depth + 2}),
    diffusionConstant(diffusionConstant), viscosity(viscosity),
    density(fullTensorDim), velocity(fullTensorDim),
    densityPrev(fullTensorDim), velocityPrev(fullTensorDim) {}

void FluidSystem::step(const DyeField &addedDensity, const VelocityField &addedVelocity,
                       Scalar dt) {
    stepVelocity(dt, addedVelocity);
    stepDensity(dt, addedDensity);
}

void FluidSystem::clear() {
    density.clear();
    velocity.clear();
    densityPrev.clear();
    velocityPrev.clear();
}

void FluidSystem::stepDensity(Scalar dt, const DyeField &addedDensity) {
    density += addedDensity;
    std::array<BoundarySetter, density.coords> boundarySetters;
    for (std::size_t i = 0; i < density.coords; ++i) {
        boundarySetters[i] = std::bind(&setContinuityBoundaries,
                                       std::placeholders::_1, dim);
    }

    std::swap(density, densityPrev);
    diffuse(density, densityPrev, diffusionConstant, dt, boundarySetters);
    std::swap(density, densityPrev);
    advect(density, densityPrev, velocity, dt, boundarySetters);
}

void FluidSystem::stepVelocity(Scalar dt, const VelocityField &addedVelocity) {
    velocity += addedVelocity;
    std::array<BoundarySetter, velocity.coords> boundarySetters;
    boundarySetters[0] = std::bind(&setHorizontalNeumannBoundaries,
                                   std::placeholders::_1, dim);
    boundarySetters[1] = std::bind(&setVerticalNeumannBoundaries,
                                   std::placeholders::_1, dim);
    boundarySetters[2] = std::bind(&setDepthNeumannBoundaries,
                                   std::placeholders::_1, dim);

    std::swap(velocity, velocityPrev);
    diffuse(velocity, velocityPrev, viscosity, dt, boundarySetters);
    project(velocity);

    std::swap(velocity, velocityPrev);
    advect(velocity, velocityPrev, velocityPrev, dt, boundarySetters);
    project(velocity);
}

void FluidSystem::project(VelocityField &velocity) const {
    Grid pressure(fullTensorDim);
    Grid divergence(fullTensorDim);
    div(divergence, velocity, dim);
    divergence = -1 * divergence;
    setContinuityBoundaries(divergence, dim);
    setContinuityBoundaries(pressure, dim);
    linearSolve(pressure, divergence, 1, 6, dim, fullTensorDim,
                std::bind(&setContinuityBoundaries, std::placeholders::_1, dim));
    VelocityField gradient(fullTensorDim);
    grad(gradient, pressure, dim);
    velocity -= gradient;
    setHorizontalNeumannBoundaries(velocity[0], dim);
    setVerticalNeumannBoundaries(velocity[1], dim);
    setDepthNeumannBoundaries(velocity[2], dim);
}

void grad(VelocityField &out, const Grid &in, const Indices &dim) {
    for (Grid::Index i = 1; i <= dim(0); ++i) {
        for (Grid::Index j = 1; j <= dim(1); ++j) {
            for (Grid::Index k = 1; k <= dim(2); ++k) {
                out[0](i, j, k) = 0.5 * (in(i + 1, j, k) - in(i - 1, j, k));
                out[1](i, j, k) = 0.5 * (in(i, j + 1, k) - in(i, j - 1, k));
                out[2](i, j, k) = 0.5 * (in(i, j, k + 1) - in(i, j, k - 1));
            }
        }
    }
}
void div(Grid &out, const VelocityField &in, const Indices &dim) {
    for (Grid::Index i = 1; i <= dim(0); ++i) {
        for (Grid::Index j = 1; j <= dim(1); ++j) {
            for (Grid::Index k = 1; k <= dim(2); ++k) {
                out(i, j, k) = 0;
                out(i, j, k) += 0.5 * (in[0](i + 1, j, k) - in[0](i - 1, j, k));
                out(i, j, k) += 0.5 * (in[1](i, j + 1, k) - in[1](i, j - 1, k));
                out(i, j, k) += 0.5 * (in[1](i, j, k + 1) - in[1](i, j, k - 1));
            }
        }
    }
}
