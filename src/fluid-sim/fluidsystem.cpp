#include "fluidsystem.h"

#include <utility>
#include <algorithm>

FluidSystem::FluidSystem(const Grid::Index gridSize, Scalar gridLength,
                         Scalar diffusionConstant, Scalar viscosity) :
    gridSize(gridSize), fullGridSize(gridSize + 2),
    gridLength(gridLength), gridSpacing(gridLength / gridSize),
    diffusionConstant(diffusionConstant), viscosity(viscosity),
    density(gridSize + 2, gridSize + 2), velocity(gridSize + 2, gridSize + 2),
    densityPrev(gridSize + 2, gridSize + 2), velocityPrev(gridSize + 2, gridSize + 2) {}

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
    Grid temp = Grid::Zero(fullGridSize, fullGridSize);

    x = x_0;
    for (unsigned int iteration = 0; iteration < numIterations; ++iteration) {
        for (Grid::Index i = 1; i <= gridSize; ++i) {
            for (Grid::Index j = 1; j <= gridSize; ++j) {
                temp(i, j) = (x_0(i, j) + a * (x(i - 1, j) + x(i + 1, j) +
                                               x(i, j - 1) + x(i, j + 1))) / c;
            }
        }
        x = temp;

        setBoundaries(x);
   }
}

void FluidSystem::project(VelocityField &velocity) const {
    Grid p = Grid::Zero(fullGridSize, fullGridSize);
    Grid div = Grid::Zero(fullGridSize, fullGridSize);
    for (Grid::Index i = 1; i <= gridSize; ++i) {
        for(Grid::Index j = 1; j <= gridSize; ++j) {
            div(i, j) = -0.5 * gridSpacing * (velocity[0](i + 1, j) - velocity[0](i - 1, j) +
                                              velocity[1](i, j + 1) - velocity[1](i, j - 1));
        }
    }
    setContinuityBoundaries(div);
    setContinuityBoundaries(p);
    solvePoisson(p, div, 1, 4,
                 std::bind(&FluidSystem::setContinuityBoundaries, this,
                           std::placeholders::_1));
    for (Grid::Index i = 1; i <= gridSize; ++i) {
        for(Grid::Index j = 1; j <= gridSize; ++j) {
            velocity[0](i, j) -= 0.5 * gridSize * (p(i + 1, j) - p(i - 1, j));
            velocity[1](i, j) -= 0.5 * gridSize * (p(i, j + 1) - p(i, j - 1));
        }
    }
    setHorizontalNeumannBoundaries(velocity[0]);
    setVerticalNeumannBoundaries(velocity[1]);
}

void FluidSystem::setBoundaries(Grid &grid, int b) const {
    for (Grid::Index i = 1; i <= gridSize; ++i) {
        grid(i, 0) = (b == 2 ? -1 : 1) * grid(i, 1);
        grid(i, gridSize + 1) = (b == 2 ? -1 : 1) * grid(i, gridSize);
    }
    for(Grid::Index j = 1; j <= gridSize; ++j) {
        grid(0, j) = (b == 1 ? -1 : 1) * grid(1, j);
        grid(gridSize + 1, j) = (b == 1 ? -1 : 1) * grid(gridSize, j);
    }
}
void FluidSystem::setContinuityBoundaries(Grid &grid) const {
    setBoundaries(grid, 0);
}
void FluidSystem::setVerticalNeumannBoundaries(Grid &grid) const {
    setBoundaries(grid, 2);
}
void FluidSystem::setHorizontalNeumannBoundaries(Grid &grid) const {
    setBoundaries(grid, 1);
}
