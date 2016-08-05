#include "vectorfield.h"

template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>::VectorField(const TensorIndices &dimensions) {
    for (auto &grid : grids) {
      grid = Grid(dimensions);
    }
    clear();
}
template<Grid::Index numStaggers, std::size_t numCoords>
void VectorField<numStaggers, numCoords>::clear() {
    for (auto &grid : grids) {
        grid.setZero();
    }
}
template<Grid::Index numStaggers, std::size_t numCoords>
const Grid &VectorField<numStaggers, numCoords>::operator[](std::size_t coord) const {
    return grids[coord];
}
template<Grid::Index numStaggers, std::size_t numCoords>
Grid &VectorField<numStaggers, numCoords>::operator[](std::size_t coord) {
    return grids[coord];
}
template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
&VectorField<numStaggers, numCoords>::operator+=(const VectorField<numStaggers, numCoords> &rhs) {
    for (std::size_t i = 0; i < numCoords; ++i) {
        grids[i] += rhs.grids[i];
    }
    return *this;
}
template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
&VectorField<numStaggers, numCoords>::operator-=(const VectorField<numStaggers, numCoords> &rhs) {
    for (std::size_t i = 0; i < numCoords; ++i) {
        grids[i] -= rhs.grids[i];
    }
    return *this;
}
template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
&VectorField<numStaggers, numCoords>::operator*=(Scalar rhs) {
    for (std::size_t i = 0; i < numCoords; ++i) {
        grids[i] = grids[i] * rhs;
    }
    return *this;
}

template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
operator+(VectorField<numStaggers, numCoords> lhs,
          const VectorField<numStaggers, numCoords> &rhs) {
    lhs += rhs;
    return lhs;
}
template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
operator-(VectorField<numStaggers, numCoords> lhs,
          const VectorField<numStaggers, numCoords> &rhs) {
    lhs -= rhs;
    return lhs;
}
template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
operator*(VectorField<numStaggers, numCoords> lhs, Scalar rhs) {
    lhs *= rhs;
    return lhs;
}
template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
operator*(Scalar lhs, VectorField<numStaggers, numCoords> rhs) {
    rhs *= lhs;
    return rhs;
}
