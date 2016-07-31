template<std::size_t numCoords>
void FluidSystem::advect(VectorField<numCoords> &out, const VectorField<numCoords> &in,
                         const VelocityField &velocity, Scalar dt,
                         std::array<BoundarySetter, numCoords> boundarySetters) const {
    for (std::size_t d = 0; d < numCoords; ++d) {
      for (Grid::Index i = 1; i <= width; ++i) {
        for (Grid::Index j = 1; j <= height; ++j) {
          Location x = {static_cast<Scalar>(i), static_cast<Scalar>(j), 0.0f};
          Location dim = dimensions.cast<Scalar>();
          Location v = {velocity[0](i, j, 0), velocity[1](i, j, 0), 0.0f};
          x = x - dt * v;
          x.min(dim + 0.5f);
          x.max(0.5f);
          x(2) = 0;
          out[d](i, j, 0) = interpolate(in[d], x);
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
      solvePoisson(out[d], in[d], a, 1 + 4 * a, boundarySetters[d]);
    }
}
