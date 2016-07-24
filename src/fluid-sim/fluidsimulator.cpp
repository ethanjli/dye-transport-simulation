#include "fluidsimulator.h"

#include <utility>
#include <algorithm>

FluidSimulator::FluidSimulator(double dt) :
    dt(dt)
{
    system = FluidSystem();
}

void FluidSimulator::addDensity() {
    system.density += system.density_add;
}
void FluidSimulator::addVelocity() {
    system.u += system.u_add;
    system.v += system.v_add;
}

void FluidSimulator::stepSystem() {
    stepVelocity();
    stepDensity();
}

void FluidSimulator::stepDensity() {
    addDensity();
    std::swap(system.density, system.density_prev);
    diffuseField(system.density, system.density_prev,
                 system.diffusionConstant, setContinuityFieldBoundaries, dt);
    std::swap(system.density, system.density_prev);
    advectField(system.density, system.density_prev,
                system.u, system.v, setContinuityFieldBoundaries, dt);
}

void FluidSimulator::stepVelocity() {
    addVelocity();
    std::swap(system.u, system.u_prev);
    diffuseField(system.u, system.u_prev, system.viscosity,
                 setHorizontalNeumannFieldBoundaries, dt);
    std::swap(system.v, system.v_prev);
    diffuseField(system.v, system.v_prev, system.viscosity,
                 setVerticalNeumannFieldBoundaries, dt);
    projectField(system.u, system.v,
                       system.u_prev, system.v_prev);
    std::swap(system.u, system.u_prev);
    std::swap(system.v, system.v_prev);
    advectField(system.u, system.u_prev, system.u_prev, system.v_prev,
                setHorizontalNeumannFieldBoundaries, dt);
    advectField(system.v, system.v_prev, system.u_prev, system.v_prev,
                setVerticalNeumannFieldBoundaries, dt);
    projectField(system.u, system.v, system.u_prev, system.v_prev);
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
            x = std::max(0.5, std::min(kGridSize + 0.5, x));
            int i_0 = (int)x;
            int i_1 = i_0 + 1;
            Scalar s_1 = x - i_0;
            Scalar s_0 = 1 - s_1;
            Scalar y = j - dt_0 * v(i, j);
            y = std::max(0.5, std::min(kGridSize + 0.5, y));
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
