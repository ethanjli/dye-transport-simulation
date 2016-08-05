#include "fluidsystem.h"

template<Grid::Index numStaggers, std::size_t numCoords>
void FluidSystem::advect(VectorField<numStaggers, numCoords> &out,
                         const VectorField<numStaggers, numCoords> &in,
                         const VelocityField &velocity, Scalar dt, const Indices &dim,
                         std::array<BoundarySetter, numCoords> boundarySetters) const {
    backtrace(out, in, velocity, dt, dim);
    for (std::size_t d = 0; d < numCoords; ++d) {
        boundarySetters[d](out[d]);
    }
    VectorField<numStaggers, numCoords> outCompensation = out;
    backtrace(outCompensation, out, velocity, -1 * dt, dim);
    for (std::size_t d = 0; d < numCoords; ++d) {
        boundarySetters[d](outCompensation[d]);
    }
    out = in + 0.5 * (in - outCompensation);
    for (std::size_t d = 0; d < numCoords; ++d) {
        boundarySetters[d](out[d]);
    }
    backtrace(out, in, velocity, dt, dim);
}
template<Grid::Index numStaggers, std::size_t numCoords>
void FluidSystem::backtrace(VectorField<numStaggers, numCoords> &out,
                            const VectorField<numStaggers, numCoords> &in,
                            const VelocityField &velocity, Scalar dt, const Indices &dim) const {
    for (std::size_t d = 0; d < numCoords; ++d) {
        for (Grid::Index i = 1; i <= dim(0); ++i) {
            for (Grid::Index j = 1; j <= dim(1); ++j) {
                for (Grid::Index k = 1; k <= dim(2); ++k) {
                    // Backtrack to the midpoint of RK2
                    Location x = { // position relative to the frame of the field
                      static_cast<Scalar>(i), static_cast<Scalar>(j),
                      static_cast<Scalar>(k)
                    };
                    Location v;
                    for (Location::Index l = 0; l < kGridDimensions; ++l) {
                        if (l < numStaggers) {
                            // average the (face-centered) velocities to get
                            // cell-centered velocity since out[l] is cell-centered
                            if (l == 0) {
                                v[l] = (velocity[l](i, j, k) + velocity[l](i + 1, j, k)) / 2;
                            } else if (l == 1) {
                                v[l] = (velocity[l](i, j, k) + velocity[l](i, j + 1, k)) / 2;
                            } else if (l == 2) {
                                v[l] = (velocity[l](i, j, k) + velocity[l](i, j, k + 1)) / 2;
                            }
                        } else {
                            // use face-centered velocity, since out[l] is face-centered
                            v[l] = velocity[l](i, j, k);
                        }
                    }
                    //x -= dt * v;
                    Location xMidpoint = x - 0.5 * dt * v;
                    // Clamp the midpoint position relative to the frame of the field
                    xMidpoint = xMidpoint.min(dim.cast<Scalar>() + 0.5f).max(0.5);
                    // Find the velocity at the RK2 midpoint
                    Location velocityMidpoint;
                    for (Location::Index l = 0; l < kGridDimensions; ++l) {
                        velocityMidpoint[l] = interpolate(velocity[l], xMidpoint);
                    }
                    // Interpolate at the final position relative to the frame of the field
                    x = x - dt * velocityMidpoint;
                    x = x.min(dim.cast<Scalar>() + 0.5f).max(0.5);
                    out[d](i, j, k) = interpolate(in[d], x);
                }
            }
        }
    }
}

template<Grid::Index numStaggers, std::size_t numCoords>
void FluidSystem::diffuse(VectorField<numStaggers, numCoords> &out,
                          const VectorField<numStaggers, numCoords> &in,
                          Scalar diff, Scalar dt, const Indices &dim,
                          std::array<BoundarySetter, numCoords> boundarySetters) const {
    Scalar a = dt * diff;
    for (std::size_t d = 0; d < numCoords; ++d) {
      linearSolve(out[d], in[d], a, 1 + 6 * a, dim, boundarySetters[d]);
    }
}
