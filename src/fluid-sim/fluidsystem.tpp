template<std::size_t numCoords>
void FluidSystem::advect(VectorField<numCoords> &out, const VectorField<numCoords> &in,
                         const VelocityField &velocity, Scalar dt,
                         std::array<BoundarySetter, numCoords> boundarySetters) const {
    for (std::size_t d = 0; d < numCoords; ++d) {
        for (Grid::Index i = 1; i <= dim(0); ++i) {
            for (Grid::Index j = 1; j <= dim(1); ++j) {
                for (Grid::Index k = 1; k <= dim(2); ++k) {
                    Location x = {static_cast<Scalar>(i), static_cast<Scalar>(j), static_cast<Scalar>(k)};
                    Location v = {velocity[0](i, j, k), velocity[1](i, j, k), velocity[2](i, j, k)};
                    x = x - dt * v;
                    x = x.min(dim.cast<Scalar>() + 0.5f);
                    x = x.max(0.5f);
                    out[d](i, j, k) = interpolate(in[d], x);
                }
            }
        }
        boundarySetters[d](out[d]);
    }
}

template<std::size_t numCoords>
void FluidSystem::diffuse(VectorField<numCoords> &out, const VectorField<numCoords> &in,
                          Scalar diff, Scalar dt,
                          std::array<BoundarySetter, numCoords> boundarySetters) const {
    Scalar a = dt * diff;
    for (std::size_t d = 0; d < numCoords; ++d) {
      solvePoisson(out[d], in[d], a, 1 + 6 * a, boundarySetters[d]);
    }
}
