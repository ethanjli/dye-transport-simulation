#include "fluidsimulator.h"

#include <utility>
#include <algorithm>

FluidSimulator::FluidSimulator(double dt) :
    dt(dt)
{
    this->system = FluidSystem();
}

void FluidSimulator::addDensity() {
    this->system.density += this->system.density_add;
}
void FluidSimulator::addVelocity() {
    this->system.u += this->system.u_add;
    this->system.v += this->system.v_add;
}

void FluidSimulator::stepSystem() {
    this->stepVelocity();
    this->stepDensity();
}

void FluidSimulator::stepDensity() {
    this->addDensity();
    std::swap(this->system.density, this->system.density_prev);
    diffuseField(this->system.density, this->system.density_prev,
                 this->system.diffusionConstant, setContinuityFieldBoundaries,
                 this->dt);
    std::swap(this->system.density, this->system.density_prev);
    advectField(this->system.density, this->system.density_prev,
                this->system.u, this->system.v, setContinuityFieldBoundaries,
                this->dt);
}

void FluidSimulator::stepVelocity() {
    this->addVelocity();
    std::swap(this->system.u, this->system.u_prev);
    diffuseField(this->system.u, this->system.u_prev, this->system.viscosity,
                 setHorizontalNeumannFieldBoundaries, this->dt);
    std::swap(this->system.v, this->system.v_prev);
    diffuseField(this->system.v, this->system.v_prev, this->system.viscosity,
                 setVerticalNeumannFieldBoundaries, this->dt);
    projectField(this->system.u, this->system.v,
                       this->system.u_prev, this->system.v_prev);
    std::swap(this->system.u, this->system.u_prev);
    std::swap(this->system.v, this->system.v_prev);
    advectField(this->system.u, this->system.u_prev,
                this->system.u_prev, this->system.v_prev,
                setHorizontalNeumannFieldBoundaries, this->dt);
    advectField(this->system.v, this->system.v_prev,
                this->system.u_prev, this->system.v_prev,
                setVerticalNeumannFieldBoundaries, this->dt);
    projectField(this->system.u, this->system.v,
                       this->system.u_prev, this->system.v_prev);
}

void solvePoisson(Grid &x, const Grid &x_0, Scalar a, Scalar c,
                  std::function<void(Grid&)> setBoundaries,
                  unsigned int numIterations) {
    Grid temp;

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
        for(Grid::Index j = 1; j <= kGridSize; ++j) {
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
    p = Grid::Zero();
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
