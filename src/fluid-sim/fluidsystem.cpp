#include "fluidsystem.h"

#include <utility>
#include <algorithm>

FluidSystem::FluidSystem(Scalar diffusionConstant, Scalar viscosity) :
    diffusionConstant(diffusionConstant), viscosity(viscosity)
{
}

void FluidSystem::step(Scalar dt, const Grid &addedDensity,
                       const Grid &addedU, const Grid &addedV) {
    stepVelocity(dt, addedU, addedV);
    stepDensity(dt, addedDensity);
}

void FluidSystem::clear() {
    u = Grid::Zero(kFullGridSize, kFullGridSize);
    v = Grid::Zero(kFullGridSize, kFullGridSize);
    u_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    v_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    density = Grid::Zero(kFullGridSize, kFullGridSize);
    density_prev = Grid::Zero(kFullGridSize, kFullGridSize);
}

void FluidSystem::stepDensity(Scalar dt, const Grid &addedDensity) {
   density += addedDensity;
    std::swap(density, density_prev);
    diffuseField(density, density_prev, diffusionConstant,
                 setContinuityFieldBoundaries, dt);
    std::swap(density, density_prev);
    advectField(density, density_prev, u, v, setContinuityFieldBoundaries, dt);
}

void FluidSystem::stepVelocity(Scalar dt, const Grid &addedU, const Grid &addedV) {
    u += addedU;
    v += addedV;
    std::swap(u, u_prev);
    diffuseField(u, u_prev, viscosity, setHorizontalNeumannFieldBoundaries, dt);
    std::swap(v, v_prev);
    diffuseField(v, v_prev, viscosity, setVerticalNeumannFieldBoundaries, dt);
    projectField(u, v, u_prev, v_prev);
    std::swap(u, u_prev);
    std::swap(v, v_prev);
    advectField(u, u_prev, u_prev, v_prev, setHorizontalNeumannFieldBoundaries, dt);
    advectField(v, v_prev, u_prev, v_prev, setVerticalNeumannFieldBoundaries, dt);
    projectField(u, v, u_prev, v_prev);
}

void solvePoisson(Grid &x, const Grid &x_0, Scalar a, Scalar c,
                  std::function<void(Grid&)> setBoundaries,
                  unsigned int numIterations) {
    Grid temp = Grid::Zero(kFullGridSize, kFullGridSize);

    x = x_0;
    for (unsigned int iteration = 0; iteration < numIterations; ++iteration) {
        for (Grid::Index i = 1; i <= kGridSize; ++i) {
            for (Grid::Index j = 1; j <= kGridSize; ++j) {
                temp(i, j) = (x_0(i, j) + a * (x(i - 1, j) + x(i + 1, j) +
                                               x(i, j - 1) + x(i, j + 1))) / c;
            }
        }
        x = temp;

        setBoundaries(x);
   }
}

void projectField(Grid &u, Grid &v, Grid &p, Grid &div) {
    p = Grid::Zero(kFullGridSize, kFullGridSize);
    for (Grid::Index i = 1; i <= kGridSize; ++i) {
        for(Grid::Index j = 1; j <= kGridSize; ++j) {
            div(i, j) = -0.5 * kGridSpacing * (u(i + 1, j) - u(i - 1, j) +
                                               v(i, j + 1) - v(i, j - 1));
        }
    }
    setContinuityFieldBoundaries(div);
    setContinuityFieldBoundaries(p);
    solvePoisson(p, div, 1, 4, setContinuityFieldBoundaries);
    for (Grid::Index i = 1; i <= kGridSize; ++i) {
        for(Grid::Index j = 1; j <= kGridSize; ++j) {
            u(i, j) -= 0.5 * kGridSize * (p(i + 1, j) - p(i - 1, j));
            v(i, j) -= 0.5 * kGridSize * (p(i, j + 1) - p(i, j - 1));
        }
    }
    setHorizontalNeumannFieldBoundaries(u);
    setVerticalNeumannFieldBoundaries(v);
}

void diffuseField(Grid &x, const Grid &x_0, Scalar diff,
                  std::function<void(Grid&)> setBoundaries, Scalar dt) {
    Scalar a = dt * diff * kGridSize * kGridSize;
    solvePoisson(x, x_0, a, 1 + 4 * a, setBoundaries);
}
void advectField(Grid &newField, const Grid &field, const Grid &u, const Grid &v,
                 std::function<void(Grid&)> setBoundaries, Scalar dt) {
    Scalar dt_0 = dt * kGridSize;
    for (Grid::Index i = 1; i <= kGridSize; ++i) {
        for (Grid::Index j = 1; j <= kGridSize; ++j) {
            Scalar x = i - dt_0 * u(i, j);
            x = std::max(0.5f, std::min(kGridSize + 0.5f, x));
            int i_0 = (int)x;
            int i_1 = i_0 + 1;
            Scalar s_1 = x - i_0;
            Scalar s_0 = 1 - s_1;
            Scalar y = j - dt_0 * v(i, j);
            y = std::max(0.5f, std::min(kGridSize + 0.5f, y));
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

void setFieldBoundaries(Grid &grid, int b) {
    for (Grid::Index i = 1; i <= kGridSize; ++i) {
        grid(i, 0) = (b == 2 ? -1 : 1) * grid(i, 1);
        grid(i, kGridSize + 1) = (b == 2 ? -1 : 1) * grid(i, kGridSize);
    }
    for(Grid::Index j = 1; j <= kGridSize; ++j) {
        grid(0, j) = (b == 1 ? -1 : 1) * grid(1, j);
        grid(kGridSize + 1, j) = (b == 1 ? -1 : 1) * grid(kGridSize, j);
    }
}
void setContinuityFieldBoundaries(Grid &grid) {
    setFieldBoundaries(grid, 0);
}
void setVerticalNeumannFieldBoundaries(Grid &grid) {
    setFieldBoundaries(grid, 2);
}
void setHorizontalNeumannFieldBoundaries(Grid &grid) {
    setFieldBoundaries(grid, 1);
}
