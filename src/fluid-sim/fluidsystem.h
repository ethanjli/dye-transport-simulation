#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include <Eigen/Dense>
#include <functional>

#include "math.h"

// Adapted from Jos Stam's Stable Fluids method
// https://d2f99xq7vri1nk.cloudfront.net/legacy_app_files/pdf/GDC03.pdf

class FluidSystem
{
public:
    FluidSystem(const Grid::Index gridSize = 300, Scalar gridLength = 1,
                Scalar diffusionConstant = 0, Scalar viscosity = 0);

    const Grid::Index gridSize;
    const Grid::Index fullGridSize;
    const Scalar gridLength;
    const Scalar gridSpacing;

    Scalar diffusionConstant;
    Scalar viscosity;

    Grid u, u_prev;
    Grid v, v_prev;

    Grid density, density_prev;

    void step(const Grid &addedDensity, const Grid &addedU, const Grid &addedV,
              Scalar dt);

    void clear();

private:
    void stepDensity(Scalar dt, const Grid &addedDensity);
    void stepVelocity(Scalar dt, const Grid &addedU, const Grid &addedV);

    void solvePoisson(Grid &solution, const Grid &initial, Scalar alpha, Scalar beta,
                      std::function<void(Grid&)> setBoundaries,
                      unsigned int numIterations = 20) const;
    void diffuseField(Grid &newField, const Grid &field, Scalar diffusionConstant,
                      Scalar dt, std::function<void(Grid&)> setBoundaries) const;
    void advectField(Grid &newField, const Grid &field, const Grid &u, const Grid &v,
                     Scalar dt, std::function<void(Grid&)> setBoundaries) const;
    void projectField(Grid &u, Grid &v, Grid &pressure, Grid &divergence) const;

    void setFieldBoundaries(Grid &grid, int b) const;
    void setContinuityFieldBoundaries(Grid &grid) const;
    void setVerticalNeumannFieldBoundaries(Grid &grid) const;
    void setHorizontalNeumannFieldBoundaries(Grid &grid) const;
};

#endif // FLUIDSYSTEM_H
