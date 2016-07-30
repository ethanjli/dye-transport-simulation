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
    std::swap(density[0], densityPrev[0]);
    diffuse(density[0], densityPrev[0], diffusionConstant, dt,
                 std::bind(&FluidSystem::setContinuityBoundaries, this,
                           std::placeholders::_1));
    std::swap(density[0], densityPrev[0]);
    advect(density[0], densityPrev[0], velocity[0], velocity[1], dt,
                std::bind(&FluidSystem::setContinuityBoundaries, this,
                          std::placeholders::_1));
}

void FluidSystem::stepVelocity(Scalar dt, const VelocityField &addedVelocity) {
    velocity += addedVelocity;
    std::swap(velocity, velocityPrev);
    diffuse(velocity[0], velocityPrev[0], viscosity, dt,
                 std::bind(&FluidSystem::setHorizontalNeumannBoundaries, this,
                           std::placeholders::_1));
    diffuse(velocity[1], velocityPrev[1], viscosity, dt,
                 std::bind(&FluidSystem::setVerticalNeumannBoundaries, this,
                           std::placeholders::_1));
    project(velocity[0], velocity[1], velocityPrev[0], velocityPrev[1]);
    std::swap(velocity, velocityPrev);
    advect(velocity[0], velocityPrev[0], velocityPrev[0], velocityPrev[1], dt,
                std::bind(&FluidSystem::setHorizontalNeumannBoundaries, this,
                          std::placeholders::_1));
    advect(velocity[1], velocityPrev[1], velocityPrev[0], velocityPrev[1], dt,
                std::bind(&FluidSystem::setVerticalNeumannBoundaries, this,
                          std::placeholders::_1));
    project(velocity[0], velocity[1], velocityPrev[0], velocityPrev[1]);
}

void FluidSystem::solvePoisson(Grid &x, const Grid &x_0, Scalar a, Scalar c,
                               std::function<void(Grid&)> setBoundaries,
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

void FluidSystem::project(Grid &u, Grid &v, Grid &p, Grid &div) const {
    p = Grid::Zero(fullGridSize, fullGridSize);
    for (Grid::Index i = 1; i <= gridSize; ++i) {
        for(Grid::Index j = 1; j <= gridSize; ++j) {
            div(i, j) = -0.5 * gridSpacing * (u(i + 1, j) - u(i - 1, j) +
                                               v(i, j + 1) - v(i, j - 1));
        }
    }
    setContinuityBoundaries(div);
    setContinuityBoundaries(p);
    solvePoisson(p, div, 1, 4,
                 std::bind(&FluidSystem::setContinuityBoundaries, this,
                           std::placeholders::_1));
    for (Grid::Index i = 1; i <= gridSize; ++i) {
        for(Grid::Index j = 1; j <= gridSize; ++j) {
            u(i, j) -= 0.5 * gridSize * (p(i + 1, j) - p(i - 1, j));
            v(i, j) -= 0.5 * gridSize * (p(i, j + 1) - p(i, j - 1));
        }
    }
    setHorizontalNeumannBoundaries(u);
    setVerticalNeumannBoundaries(v);
}

void FluidSystem::diffuse(Grid &x, const Grid &x_0, Scalar diff, Scalar dt,
                          std::function<void(Grid&)> setBoundaries) const {
    Scalar a = dt * diff * gridSize * gridSize;
    solvePoisson(x, x_0, a, 1 + 4 * a, setBoundaries);
}
void FluidSystem::advect(Grid &newField, const Grid &field,
                              const Grid &u, const Grid &v, Scalar dt,
                              std::function<void(Grid&)> setBoundaries) const {
    Scalar dt_0 = dt * gridSize;
    for (Grid::Index i = 1; i <= gridSize; ++i) {
        for (Grid::Index j = 1; j <= gridSize; ++j) {
            Scalar x = i - dt_0 * u(i, j);
            x = std::max(0.5f, std::min(gridSize + 0.5f, x));
            int i_0 = (int)x;
            int i_1 = i_0 + 1;
            Scalar s_1 = x - i_0;
            Scalar s_0 = 1 - s_1;
            Scalar y = j - dt_0 * v(i, j);
            y = std::max(0.5f, std::min(gridSize + 0.5f, y));
            int j_0 = (int)y;
            int j_1 = j_0 + 1;
            Scalar t_1 = y - j_0;
            Scalar t_0 = 1 - t_1;
            newField(i, j) = (s_0 * (t_0 * field(i_0, j_0) + t_1 * field(i_0, j_1)) +
                              s_1 * (t_0 * field(i_1, j_0) + t_1 * field(i_1, j_1)));
        }
    }
    setBoundaries(newField);
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
