#ifndef FLUIDSIMULATOR_H
#define FLUIDSIMULATOR_H

#include <functional>

#include "math.h"
#include "fluidsystem.h"

class FluidSimulator
{
public:
    FluidSimulator(FluidSystem &system, double dt = 0.1);

    FluidSystem &system;
    double dt;

    void addDensity();
    void addVelocity();
    void stepSystem();

private:
    void stepDensity();
    void stepVelocity();
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
