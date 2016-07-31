#include "fluidsystem.h"

#include <utility>
#include <algorithm>

FluidSystem::FluidSystem(Grid::Index width, Grid::Index height,
                         Scalar diffusionConstant, Scalar viscosity) :
    width(width), height(height), fullWidth(width + 2), fullHeight(height + 2),
    diffusionConstant(diffusionConstant), viscosity(viscosity),
    density(width + 2, height + 2), velocity(width + 2, height + 2),
    densityPrev(width + 2, height + 2), velocityPrev(width + 2, height + 2) {}

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
        boundarySetters[i] = std::bind(&FluidSystem::setContinuityBoundaries, this,
                                       std::placeholders::_1);
    }

    std::swap(density, densityPrev);
    diffuse(density, densityPrev, diffusionConstant, dt, boundarySetters);
    std::swap(density, densityPrev);
    advect(density, densityPrev, velocity, dt, boundarySetters);
}

void FluidSystem::stepVelocity(Scalar dt, const VelocityField &addedVelocity) {
    velocity += addedVelocity;
    std::array<BoundarySetter, velocity.coords> boundarySetters;
    boundarySetters[0] = std::bind(&FluidSystem::setHorizontalNeumannBoundaries,
                                   this, std::placeholders::_1);
    boundarySetters[1] = std::bind(&FluidSystem::setVerticalNeumannBoundaries,
                                   this, std::placeholders::_1);

    std::swap(velocity, velocityPrev);
    diffuse(velocity, velocityPrev, viscosity, dt, boundarySetters);
    project(velocity);

    std::swap(velocity, velocityPrev);
    advect(velocity, velocityPrev, velocityPrev, dt, boundarySetters);
    project(velocity);
}

void FluidSystem::solvePoisson(Grid &x, const Grid &x_0, Scalar a, Scalar c,
                               BoundarySetter setBoundaries,
                               unsigned int numIterations) const {
    Grid temp(fullWidth, fullHeight);

    x = x_0;
    for (unsigned int iteration = 0; iteration < numIterations; ++iteration) {
        for (Grid::Index i = 1; i <= width; ++i) {
            for (Grid::Index j = 1; j <= height; ++j) {
                temp(i, j) = (x_0(i, j) + a * (x(i - 1, j) + x(i + 1, j) +
                                               x(i, j - 1) + x(i, j + 1))) / c;
            }
        }
        x = temp;

        setBoundaries(x);
   }
}

void FluidSystem::project(VelocityField &velocity) const {
    Grid pressure(fullWidth, fullHeight);
    Grid divergence(fullWidth, fullHeight);
    div(divergence, velocity);
    divergence = -1 * divergence;
    setContinuityBoundaries(divergence);
    setContinuityBoundaries(pressure);
    solvePoisson(pressure, divergence, 1, 4,
                 std::bind(&FluidSystem::setContinuityBoundaries, this,
                           std::placeholders::_1));
    VelocityField gradient(fullWidth, fullHeight);
    grad(gradient, pressure);
    velocity -= gradient;
    setHorizontalNeumannBoundaries(velocity[0]);
    setVerticalNeumannBoundaries(velocity[1]);
}
void FluidSystem::grad(VelocityField &out, const Grid &in) const {
    for (Grid::Index i = 1; i <= width; ++i) {
        for (Grid::Index j = 1; j <= height; ++j) {
            out[0](i, j) = 0.5 * (in(i + 1, j) - in(i - 1, j));
            out[1](i, j) = 0.5 * (in(i, j + 1) - in(i, j - 1));
        }
    }
}
void FluidSystem::div(Grid &out, const VelocityField &in) const {
    for (Grid::Index i = 1; i <= width; ++i) {
        for (Grid::Index j = 1; j <= height; ++j) {
            out(i, j) = 0.5 * (in[0](i + 1, j) - in[0](i - 1, j));
            out(i, j) += 0.5 * (in[1](i, j + 1) - in[1](i, j - 1));
        }
    }
}

void FluidSystem::setBoundaries(Grid &grid, int b) const {
    for(Grid::Index j = 1; j <= height; ++j) {
        grid(0, j) = (b == 1 ? -1 : 1) * grid(1, j);
        grid(width + 1, j) = (b == 1 ? -1 : 1) * grid(width, j);
    }
    for (Grid::Index i = 1; i <= width; ++i) {
        grid(i, 0) = (b == 2 ? -1 : 1) * grid(i, 1);
        grid(i, height + 1) = (b == 2 ? -1 : 1) * grid(i, height);
    }
}
void FluidSystem::setContinuityBoundaries(Grid &grid) const {
    setBoundaries(grid, 0);
}
void FluidSystem::setHorizontalNeumannBoundaries(Grid &grid) const {
    setBoundaries(grid, 1);
}
void FluidSystem::setVerticalNeumannBoundaries(Grid &grid) const {
    setBoundaries(grid, 2);
}
