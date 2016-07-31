template<std::size_t numCoords>
void FluidSystem::advect(VectorField<numCoords> &out, const VectorField<numCoords> &in,
                         const VelocityField &velocity, Scalar dt,
                         std::array<BoundarySetter, numCoords> boundarySetters) const {
    for (std::size_t d = 0; d < numCoords; ++d) {
      for (Grid::Index i = 1; i <= width; ++i) {
        for (Grid::Index j = 1; j <= height; ++j) {
          Scalar x = i - dt * velocity[0](i, j, 0);
          Scalar y = j - dt * velocity[1](i, j, 0);
          x = std::max(0.5f, std::min(width + 0.5f, x));
          y = std::max(0.5f, std::min(height + 0.5f, y));
          out[d](i, j, 0) = interpolate(in[d], x, y);
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
