#include "fluidsystem.h"

#include <utility>
#include <algorithm>

FluidSystem::FluidSystem(Grid::Index width, Grid::Index height, Grid::Index depth,
                         Scalar diffusionConstant, Scalar viscosity) :
    width(width), height(height), depth(depth),
    fullWidth(width + 2), fullHeight(height + 2), fullDepth(depth + 2),
    dimensions({width, height, depth}),
    fullDimensions({fullWidth, fullHeight, fullDepth}),
    diffusionConstant(diffusionConstant), viscosity(viscosity),
    density(fullDimensions), velocity(fullDimensions),
    densityPrev(fullDimensions), velocityPrev(fullDimensions) {}

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
    boundarySetters[2] = std::bind(&FluidSystem::setDepthNeumannBoundaries,
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
    x = x_0;
    if (a == 0) {
        setBoundaries(x);
        return;
    }

    Grid temp(fullDimensions);
    temp.setZero();
    for (unsigned int iteration = 0; iteration < numIterations; ++iteration) {
        for (Grid::Index i = 1; i <= width; ++i) {
            for (Grid::Index j = 1; j <= height; ++j) {
                for (Grid::Index k = 1; k <= depth; ++k) {
                    temp(i, j, k) = (x_0(i, j, k) +
                                     a * (x(i - 1, j, k) + x(i + 1, j, k) +
                                          x(i, j - 1, k) + x(i, j + 1, k) +
                                          x(i, j, k - 1) + x(i, j, k + 1))) / c;
                }
            }
        }
        x = temp;

        setBoundaries(x);
   }
}

void FluidSystem::project(VelocityField &velocity) const {
    Grid pressure(fullDimensions);
    Grid divergence(fullDimensions);
    div(divergence, velocity);
    divergence = -1 * divergence;
    setContinuityBoundaries(divergence);
    setContinuityBoundaries(pressure);
    solvePoisson(pressure, divergence, 1, 6,
                 std::bind(&FluidSystem::setContinuityBoundaries, this,
                           std::placeholders::_1));
    VelocityField gradient(fullDimensions);
    grad(gradient, pressure);
    velocity -= gradient;
    setHorizontalNeumannBoundaries(velocity[0]);
    setVerticalNeumannBoundaries(velocity[1]);
    setDepthNeumannBoundaries(velocity[2]);
}
void FluidSystem::grad(VelocityField &out, const Grid &in) const {
    for (Grid::Index i = 1; i <= width; ++i) {
        for (Grid::Index j = 1; j <= height; ++j) {
            for (Grid::Index k = 1; k <= depth; ++k) {
                out[0](i, j, k) = 0.5 * (in(i + 1, j, k) - in(i - 1, j, k));
                out[1](i, j, k) = 0.5 * (in(i, j + 1, k) - in(i, j - 1, k));
                out[2](i, j, k) = 0.5 * (in(i, j, k + 1) - in(i, j, k - 1));
            }
        }
    }
}
void FluidSystem::div(Grid &out, const VelocityField &in) const {
    for (Grid::Index i = 1; i <= width; ++i) {
        for (Grid::Index j = 1; j <= height; ++j) {
            for (Grid::Index k = 1; k <= depth; ++k) {
                out(i, j, k) = 0;
                out(i, j, k) += 0.5 * (in[0](i + 1, j, k) - in[0](i - 1, j, k));
                out(i, j, k) += 0.5 * (in[1](i, j + 1, k) - in[1](i, j - 1, k));
                out(i, j, k) += 0.5 * (in[1](i, j, k + 1) - in[1](i, j, k - 1));
            }
        }
    }
}

void FluidSystem::setBoundaries(Grid &grid, int b) const {
    for(Grid::Index j = 1; j <= height; ++j) {
        for (Grid::Index k = 1; k <= depth; ++k) {
            grid(0, j, k) = (b == 0 ? -1 : 1) * grid(1, j, k);
            grid(width + 1, j, k) = (b == 0 ? -1 : 1) * grid(width, j, k);
        }
    }
    for (Grid::Index i = 1; i <= width; ++i) {
        for (Grid::Index k = 1; k <= depth; ++k) {
            grid(i, 0, k) = (b == 1 ? -1 : 1) * grid(i, 1, k);
            grid(i, height + 1, k) = (b == 1 ? -1 : 1) * grid(i, height, k);
        }
    }
    for (Grid::Index i = 1; i <= width; ++i) {
        for (Grid::Index j = 1; j <= height; ++j) {
            grid(i, j, 0) = (b == 2 ? -1 : 1) * grid(i, j, 1);
            grid(i, j, depth + 1) = (b == 2 ? -1 : 1) * grid(i, j, depth);
        }
    }

    grid(0, 0, 0) = (grid(1, 0, 0) + grid(0, 1, 0) + grid(0, 0, 1)) / 3;
    grid(width + 1, 0, 0) = (grid(width, 0, 0) + grid(width + 1, 1, 0) +
                             grid(width + 1, 0, 1)) / 3;
    grid(0, height + 1, 0) = (grid(1, height + 1, 0) + grid(0, height, 0) +
                              grid(0, height + 1, 1)) / 3;
    grid(0, 0, depth + 1) = (grid(1, 0, depth + 1) + grid(0, 1, depth + 1) +
                             grid(0, 0, depth)) / 3;
    grid(width + 1, height + 1, 0) = (grid(width, height + 1, 0) +
                                      grid(width + 1, height, 0) +
                                      grid(width + 1, height + 1, 1)) / 3;
    grid(width + 1, 0, depth + 1) = (grid(width, 0, depth + 1) +
                                     grid(width + 1, 1, depth + 1) +
                                     grid(width + 1, 0, depth)) / 3;
    grid(0, height + 1, depth + 1) = (grid(1, height + 1, depth + 1) +
                                      grid(0, height, depth + 1) +
                                      grid(0, height + 1, depth)) / 3;
    grid(width + 1, height + 1, depth + 1) = (grid(width, height + 1, depth + 1) +
                                              grid(width + 1, height, depth + 1) +
                                              grid(width + 1, height + 1, depth)) / 3;
}
void FluidSystem::setContinuityBoundaries(Grid &grid) const {
    setBoundaries(grid, -1);
}
void FluidSystem::setHorizontalNeumannBoundaries(Grid &grid) const {
    setBoundaries(grid, 0);
}
void FluidSystem::setVerticalNeumannBoundaries(Grid &grid) const {
    setBoundaries(grid, 1);
}
void FluidSystem::setDepthNeumannBoundaries(Grid &grid) const {
    setBoundaries(grid, 2);
}
