#ifndef VECTORFIELD_H
#define VECTORFIELD_H

#include "math.h"

template<Grid::Index numStaggers, std::size_t numCoords>
class VectorField {
public:
    VectorField(const TensorIndices &dimensions);

    static const std::size_t coords = numCoords;

    void clear();

    const Grid &operator[](std::size_t coord) const;
    Grid &operator[](std::size_t coord);

    VectorField<numStaggers, numCoords>
    &operator+=(const VectorField<numStaggers, numCoords> &rhs);
    VectorField<numStaggers, numCoords>
    &operator-=(const VectorField<numStaggers, numCoords> &rhs);
    VectorField<numStaggers, numCoords> &operator*=(Scalar rhs);

private:
    std::array<Grid, numCoords> grids;

};
template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
operator-=(VectorField<numStaggers, numCoords> lhs,
           const VectorField<numStaggers, numCoords> &rhs);
template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
operator-(VectorField<numStaggers, numCoords> lhs,
          const VectorField<numStaggers, numCoords> &rhs);
template<Grid::Index numStaggers, std::size_t numCoords>
VectorField<numStaggers, numCoords>
operator*(VectorField<numStaggers, numCoords> lhs, Scalar rhs);

#include "vectorfield.tpp"

#endif // VECTORFIELD_H
