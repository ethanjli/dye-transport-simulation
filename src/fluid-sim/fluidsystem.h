#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include <Eigen/Dense>
#include <functional>

#include "math.h"

// Adapted from Jos Stam's Stable Fluids method
// https://d2f99xq7vri1nk.cloudfront.net/legacy_app_files/pdf/GDC03.pdf

// Compile-time parameters
const unsigned int kGridSize = 300;
const double kGridLength = 1;

const unsigned int kFullGridSize = kGridSize + 2;
const double kGridSpacing = kGridLength / kGridSize;
typedef Eigen::Array<Scalar, Eigen::Dynamic, Eigen::Dynamic> Grid;

class FluidSystem
{
public:
    FluidSystem(Scalar diffusionConstant = 0, Scalar viscosity = 0);

    Scalar diffusionConstant;
    Scalar viscosity;

    Grid u = Grid::Random(kFullGridSize, kFullGridSize);
    Grid v = Grid::Random(kFullGridSize, kFullGridSize);
    Grid u_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid v_prev = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid u_add = Grid::Zero(kFullGridSize, kFullGridSize);
    Grid v_add = Grid::Zero(kFullGridSize, kFullGridSize);

    Grid density = (Grid::Random(kFullGridSize, kFullGridSize) + 1) * 0.5;
    Grid density_prev = Grid::Zero(kFullGridSize, kFullGridSize);

    void step(Scalar dt, const Grid &addedDensity,
              const Grid &addedU, const Grid &addedV);

    void clear();

private:
    void stepDensity(Scalar dt, const Grid &addedDensity);
    void stepVelocity(Scalar dt, const Grid &addedU, const Grid &addedV);
};

void solvePoisson(Grid &solution, const Grid &initial, Scalar alpha, Scalar beta,
                  std::function<void(Grid&)> setBoundaries,
                  unsigned int numIterations = 20);
void diffuseField(Grid &newField, const Grid &field, Scalar diffusionConstant,
                  std::function<void(Grid&)> setBoundaries, Scalar dt);
void advectField(Grid &newField, const Grid &field, const Grid &u, const Grid &v,
                 std::function<void(Grid&)> setBoundaries, Scalar dt);
void projectField(Grid &u, Grid &v, Grid &pressure, Grid &divergence);

void setContinuityFieldBoundaries(Grid &grid);
void setVerticalNeumannFieldBoundaries(Grid &grid);
void setHorizontalNeumannFieldBoundaries(Grid &grid);

#endif // FLUIDSYSTEM_H
