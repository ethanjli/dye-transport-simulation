#include "fluidsystem.h"

#include <utility>
#include <algorithm>

FluidSystem::FluidSystem(Grid::Index width, Grid::Index height, Grid::Index depth,
                         Scalar diffusionConstant, Scalar viscosity) :
    dim({width, height, depth}), staggeredDim(dim + 1),
    fullDim({width + 2, height + 2, depth + 2}),
    fullStaggeredDim({width + 3, height + 3, depth + 3}),
    diffusionConstant(diffusionConstant), viscosity(viscosity),
    density(fullDim), velocity(fullStaggeredDim),
    densityPrev(fullDim), velocityPrev(fullStaggeredDim) {}

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
    density += addedDensity * dt;
    std::array<BoundarySetter, density.coords> boundarySetters;
    for (std::size_t i = 0; i < density.coords; ++i) {
        boundarySetters[i] = std::bind(&setContinuityBoundaries, std::placeholders::_1, dim);
    }

    std::swap(density, densityPrev);
    diffuse(density, densityPrev, diffusionConstant, dt, dim, boundarySetters);
    std::swap(density, densityPrev);
    advect(density, densityPrev, velocity, dt, dim, boundarySetters);
}

void FluidSystem::stepVelocity(Scalar dt, const VelocityField &addedVelocity) {
    velocity += addedVelocity * dt;
    std::array<BoundarySetter, velocity.coords> boundarySetters;
    if (horizontalNeumann) {
        boundarySetters[0] = std::bind(&setHorizontalNeumannBoundaries, std::placeholders::_1, dim);
    } else {
        boundarySetters[0] = std::bind(&setContinuityBoundaries, std::placeholders::_1, dim);
    }
    if (verticalNeumann) {
        boundarySetters[1] = std::bind(&setVerticalNeumannBoundaries, std::placeholders::_1, dim);
    } else {
        boundarySetters[1] = std::bind(&setContinuityBoundaries, std::placeholders::_1, dim);
    }
    boundarySetters[2] = std::bind(&setDepthNeumannBoundaries, std::placeholders::_1, dim);

    std::swap(velocity, velocityPrev);
    diffuse(velocity, velocityPrev, viscosity, dt, staggeredDim, boundarySetters);
    project(velocity);

    std::swap(velocity, velocityPrev);
    advect(velocity, velocityPrev, velocityPrev, dt, staggeredDim, boundarySetters);
    project(velocity);
}

void FluidSystem::project(VelocityField &velocity) const {
    Grid pressure(fullDim);
    Grid divergence(fullDim);
    div(divergence, velocity, dim);
    divergence = -1 * divergence;
    setContinuityBoundaries(divergence, dim);
    setContinuityBoundaries(pressure, dim);
    linearSolve(pressure, divergence, 1, 6, dim, std::bind(&setContinuityBoundaries,
                                                           std::placeholders::_1, dim));
    VelocityField gradient(fullStaggeredDim);
    gradient.clear();
    grad(gradient, pressure, dim);
    velocity -= gradient;
    if (horizontalNeumann) {
        setHorizontalNeumannBoundaries(velocity[0], dim);
    } else {
        setContinuityBoundaries(velocity[0], dim);
    }
    if (verticalNeumann) {
        setVerticalNeumannBoundaries(velocity[1], dim);
    } else {
        setContinuityBoundaries(velocity[1], dim);
    }
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
                out(i, j, k) += 0.5 * (in[2](i, j, k + 1) - in[2](i, j, k - 1));
            }
        }
    }
}
