#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include <functional>

#include "vectorfield.h"

// Adapted from Jos Stam's Stable Fluids method
// https://d2f99xq7vri1nk.cloudfront.net/legacy_app_files/pdf/GDC03.pdf

typedef VectorField<3> DyeField;
typedef VectorField<2> VelocityField;

class FluidSystem
{
public:
    FluidSystem(const Grid::Index gridSize = 200, Scalar diffusionConstant = 0,
                Scalar viscosity = 0);

    const Grid::Index gridSize;
    const Grid::Index fullGridSize;

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
    typedef std::function<void(Grid&)> BoundarySetter;

    void stepDensity(Scalar dt, const DyeField &addedDensity);
    void stepVelocity(Scalar dt, const VelocityField &addedVelocity);

    void solvePoisson(Grid &solution, const Grid &initial, Scalar alpha, Scalar beta,
                      BoundarySetter setBoundaries,
                      unsigned int numIterations = 20) const;
    template<std::size_t numCoords>
    void diffuse(VectorField<numCoords> &out, const VectorField<numCoords> &in,
                 Scalar diffusionConstant, Scalar dt,
                 std::array<BoundarySetter, numCoords> setBoundaries) const;
    template<std::size_t numCoords>
    void advect(VectorField<numCoords> &out, const VectorField<numCoords> &in,
                const VelocityField &velocity, Scalar dt,
                std::array<BoundarySetter, numCoords> setBoundaries) const;
    void project(VelocityField &u) const;
    void grad(VelocityField &out, const Grid &in) const;
    void div(Grid &out, const VelocityField &in) const;

    void setBoundaries(Grid &grid, int b) const;
    void setContinuityBoundaries(Grid &grid) const;
    void setVerticalNeumannBoundaries(Grid &grid) const;
    void setHorizontalNeumannBoundaries(Grid &grid) const;
};

#include "fluidsystem.tpp"

#endif // FLUIDSYSTEM_H
