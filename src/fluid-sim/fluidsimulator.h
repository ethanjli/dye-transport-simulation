#ifndef FLUIDSIMULATOR_H
#define FLUIDSIMULATOR_H

#include <functional>

#include "math.h"
#include "fluidsystem.h"

// Adapted from Jos Stam's Stable Fluids method
// https://d2f99xq7vri1nk.cloudfront.net/legacy_app_files/pdf/GDC03.pdf

class FluidSimulator
{
public:
    FluidSimulator(FluidSystem &system);

    FluidSystem &system;

    void addDensity();
    void addVelocity();
    void stepSystem(Scalar dt);

private:
    void stepDensity(Scalar dt);
    void stepVelocity(Scalar dt);
};

void solvePoisson(Grid &solution, const Grid &initial,
                  Scalar alpha, Scalar beta,
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

#endif // FLUIDSIMULATOR_H
