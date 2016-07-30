#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include <Eigen/Dense>
#include <functional>

#include "math.h"

// Adapted from Jos Stam's Stable Fluids method
// https://d2f99xq7vri1nk.cloudfront.net/legacy_app_files/pdf/GDC03.pdf

typedef VectorField<1> DyeField;
typedef VectorField<2> VelocityField;

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

    DyeField density;
    VelocityField velocity;

    void step(const DyeField &addedDensity, const VelocityField &addedVelocity,
              Scalar dt);

    void clear();

private:
    DyeField densityPrev;
    VelocityField velocityPrev;

    void stepDensity(Scalar dt, const DyeField &addedDensity);
    void stepVelocity(Scalar dt, const VelocityField &addedVelocity);

    void solvePoisson(Grid &solution, const Grid &initial, Scalar alpha, Scalar beta,
                      std::function<void(Grid&)> setBoundaries,
                      unsigned int numIterations = 20) const;
    void diffuse(Grid &out, const Grid &in, Scalar diffusionConstant,
                 Scalar dt, std::function<void(Grid&)> setBoundaries) const;
    void advect(Grid &out, const Grid &in, const Grid &u, const Grid &v,
                Scalar dt, std::function<void(Grid&)> setBoundaries) const;
    void project(Grid &u, Grid &v, Grid &pressure, Grid &divergence) const;

    void setBoundaries(Grid &grid, int b) const;
    void setContinuityBoundaries(Grid &grid) const;
    void setVerticalNeumannBoundaries(Grid &grid) const;
    void setHorizontalNeumannBoundaries(Grid &grid) const;
};

#endif // FLUIDSYSTEM_H
