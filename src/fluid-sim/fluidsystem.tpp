#include "fluidsystem.h"

template<Grid::Index numStaggers, std::size_t numCoords>
void FluidSystem::advect(VectorField<numStaggers, numCoords> &out,
                         const VectorField<numStaggers, numCoords> &in,
                         const VelocityField &velocity, Scalar dt, const Indices &dim,
                         std::array<BoundarySetter, numCoords> boundarySetters) const {
    for (std::size_t d = 0; d < numCoords; ++d) {
        for (Grid::Index i = 1; i <= dim(0); ++i) {
            for (Grid::Index j = 1; j <= dim(1); ++j) {
                for (Grid::Index k = 1; k <= dim(2); ++k) {
                    Location x = { // position relative to the frame of the field
                      static_cast<Scalar>(i), static_cast<Scalar>(j),
                      static_cast<Scalar>(k)
                    };
                    Location v;
                    for (Location::Index l = 0; l < kGridDimensions; ++l) {
                        if (l < numStaggers) {
                            // average the (face-centered) velocities to get
                            // cell-centered velocity since out[l] is cell-centered
                            if (l == 0) { // TODO: this shouldn't be hard-coded...
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
                    x = x - dt * v;
                    // Clamp the new position relative to the frame of the field
                    x = x.min(dim.cast<Scalar>() + 0.5f);
                    x = x.max(0.5);
                    // Interpolate relative to the frame of the field
                    out[d](i, j, k) = interpolate(in[d], x);
                }
            }
        }
        boundarySetters[d](out[d]);
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
