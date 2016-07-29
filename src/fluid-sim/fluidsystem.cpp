#include "fluidsystem.h"

#include <utility>
#include <algorithm>

FluidSystem::FluidSystem(const Grid::Index gridSize, Scalar gridLength,
                         Scalar diffusionConstant, Scalar viscosity) :
    gridSize(gridSize), fullGridSize(gridSize + 2),
    gridLength(gridLength), gridSpacing(gridLength / gridSize),
    diffusionConstant(diffusionConstant), viscosity(viscosity) {
    clear();
}

void FluidSystem::step(const Grid &addedDensity, const Grid &addedU, const Grid &addedV,
                       Scalar dt) {
    stepVelocity(dt, addedU, addedV);
    stepDensity(dt, addedDensity);
}

void FluidSystem::clear() {
    u = Grid::Zero(fullGridSize, fullGridSize);
    v = Grid::Zero(fullGridSize, fullGridSize);
    u_prev = Grid::Zero(fullGridSize, fullGridSize);
    v_prev = Grid::Zero(fullGridSize, fullGridSize);
    density = Grid::Zero(fullGridSize, fullGridSize);
    density_prev = Grid::Zero(fullGridSize, fullGridSize);
}

void FluidSystem::stepDensity(Scalar dt, const Grid &addedDensity) {
   density += addedDensity;
    std::swap(density, density_prev);
    diffuseField(density, density_prev, diffusionConstant, dt,
                 std::bind(&FluidSystem::setContinuityFieldBoundaries, this,
                           std::placeholders::_1));
    std::swap(density, density_prev);
    advectField(density, density_prev, u, v, dt,
                std::bind(&FluidSystem::setContinuityFieldBoundaries, this,
                          std::placeholders::_1));
}

void FluidSystem::stepVelocity(Scalar dt, const Grid &addedU, const Grid &addedV) {
    u += addedU;
    v += addedV;
    std::swap(u, u_prev);
    diffuseField(u, u_prev, viscosity, dt,
                 std::bind(&FluidSystem::setHorizontalNeumannFieldBoundaries, this,
                           std::placeholders::_1));
    std::swap(v, v_prev);
    diffuseField(v, v_prev, viscosity, dt,
                 std::bind(&FluidSystem::setVerticalNeumannFieldBoundaries, this,
                           std::placeholders::_1));
    projectField(u, v, u_prev, v_prev);
    std::swap(u, u_prev);
    std::swap(v, v_prev);
    advectField(u, u_prev, u_prev, v_prev, dt,
                std::bind(&FluidSystem::setHorizontalNeumannFieldBoundaries, this,
                          std::placeholders::_1));
    advectField(v, v_prev, u_prev, v_prev, dt,
                std::bind(&FluidSystem::setVerticalNeumannFieldBoundaries, this,
                          std::placeholders::_1));
    projectField(u, v, u_prev, v_prev);
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

void FluidSystem::projectField(Grid &u, Grid &v, Grid &p, Grid &div) const {
    p = Grid::Zero(fullGridSize, fullGridSize);
    for (Grid::Index i = 1; i <= gridSize; ++i) {
        for(Grid::Index j = 1; j <= gridSize; ++j) {
            div(i, j) = -0.5 * gridSpacing * (u(i + 1, j) - u(i - 1, j) +
                                               v(i, j + 1) - v(i, j - 1));
        }
    }
    setContinuityFieldBoundaries(div);
    setContinuityFieldBoundaries(p);
    solvePoisson(p, div, 1, 4,
                 std::bind(&FluidSystem::setContinuityFieldBoundaries, this,
                           std::placeholders::_1));
    for (Grid::Index i = 1; i <= gridSize; ++i) {
        for(Grid::Index j = 1; j <= gridSize; ++j) {
            u(i, j) -= 0.5 * gridSize * (p(i + 1, j) - p(i - 1, j));
            v(i, j) -= 0.5 * gridSize * (p(i, j + 1) - p(i, j - 1));
        }
    }
    setHorizontalNeumannFieldBoundaries(u);
    setVerticalNeumannFieldBoundaries(v);
}

void FluidSystem::diffuseField(Grid &x, const Grid &x_0, Scalar diff, Scalar dt,
                               std::function<void(Grid&)> setBoundaries) const {
    Scalar a = dt * diff * gridSize * gridSize;
    solvePoisson(x, x_0, a, 1 + 4 * a, setBoundaries);
}
void FluidSystem::advectField(Grid &newField, const Grid &field,
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

void FluidSystem::setFieldBoundaries(Grid &grid, int b) const {
    for (Grid::Index i = 1; i <= gridSize; ++i) {
        grid(i, 0) = (b == 2 ? -1 : 1) * grid(i, 1);
        grid(i, gridSize + 1) = (b == 2 ? -1 : 1) * grid(i, gridSize);
    }
    for(Grid::Index j = 1; j <= gridSize; ++j) {
        grid(0, j) = (b == 1 ? -1 : 1) * grid(1, j);
        grid(gridSize + 1, j) = (b == 1 ? -1 : 1) * grid(gridSize, j);
    }
}
void FluidSystem::setContinuityFieldBoundaries(Grid &grid) const {
    setFieldBoundaries(grid, 0);
}
void FluidSystem::setVerticalNeumannFieldBoundaries(Grid &grid) const {
    setFieldBoundaries(grid, 2);
}
void FluidSystem::setHorizontalNeumannFieldBoundaries(Grid &grid) const {
    setFieldBoundaries(grid, 1);
}
