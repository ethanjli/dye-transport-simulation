#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include <functional>

#include "vectorfield.h"

// Adapted from Jos Stam's Stable Fluids method
// https://d2f99xq7vri1nk.cloudfront.net/legacy_app_files/pdf/GDC03.pdf

typedef VectorField<3> DyeField;
typedef VectorField<3> VelocityField;

class FluidSystem
{
public:
    FluidSystem(Grid::Index width = 40, Grid::Index height = 40, Grid::Index depth = 5,
                Scalar diffusionConstant = 0, Scalar viscosity = 0);

    // Grid dimensions
    const Indices dim, fullDim;
    const TensorIndices fullTensorDim;

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

    template<std::size_t numCoords>
    void diffuse(VectorField<numCoords> &out, const VectorField<numCoords> &in,
                 Scalar diffusionConstant, Scalar dt,
                 std::array<BoundarySetter, numCoords> setBoundaries) const;
    template<std::size_t numCoords>
    void advect(VectorField<numCoords> &out, const VectorField<numCoords> &in,
                const VelocityField &velocity, Scalar dt,
                std::array<BoundarySetter, numCoords> setBoundaries) const;
    void project(VelocityField &u) const;
};

void grad(VelocityField &out, const Grid &in, const Indices &dim);
void div(Grid &out, const VelocityField &in, const Indices &dim);

#include "fluidsystem.tpp"

#endif // FLUIDSYSTEM_H
