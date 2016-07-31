template<std::size_t numCoords>
VectorField<numCoords>::VectorField(const std::array<Eigen::Index, kGridDimensions> &dimensions) {
    for (auto &grid : grids) {
        grid = Grid(dimensions);
        grid.setZero();
    }
}

template<std::size_t numCoords>
void VectorField<numCoords>::clear() {
    for (auto &grid : grids) {
        grid.setZero();
    }
}

template<std::size_t numCoords>
const Grid &VectorField<numCoords>::operator[](std::size_t i) const {
    return grids[i];
}
template<std::size_t numCoords>
Grid &VectorField<numCoords>::operator[](std::size_t i) {
    return grids[i];
}

template<std::size_t numCoords>
VectorField<numCoords> &VectorField<numCoords>::operator+=(const VectorField<numCoords> &rhs) {
    for (std::size_t i = 0; i < numCoords; ++i) {
        grids[i] += rhs.grids[i];
    }
    return *this;
}
template<std::size_t numCoords>
VectorField<numCoords> &VectorField<numCoords>::operator-=(const VectorField<numCoords> &rhs) {
    for (std::size_t i = 0; i < numCoords; ++i) {
        grids[i] -= rhs.grids[i];
    }
    return *this;
}
template<std::size_t numCoords>
VectorField<numCoords> &VectorField<numCoords>::operator*=(Scalar rhs) {
    for (std::size_t i = 0; i < numCoords; ++i) {
        grids[i] *= rhs;
    }
    return *this;
}

template<std::size_t numCoords>
VectorField<numCoords> operator+(VectorField<numCoords> lhs,
                                 const VectorField<numCoords> &rhs) {
    lhs += rhs;
    return lhs;
}
template<std::size_t numCoords>
VectorField<numCoords> operator-(VectorField<numCoords> lhs,
                                 const VectorField<numCoords> &rhs) {
    lhs -= rhs;
    return lhs;
}
template<std::size_t numCoords>
VectorField<numCoords> operator*(VectorField<numCoords> lhs, Scalar rhs) {
    lhs *= rhs;
    return lhs;
}

