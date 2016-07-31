#ifndef VECTORFIELD_H
#define VECTORFIELD_H

#include "math.h"

template<std::size_t numCoords>
class VectorField {
public:
    VectorField(Grid::Index numRows, Grid::Index numCols);

    static const std::size_t coords = numCoords;

    void clear();

    const Grid &operator[](std::size_t i) const;
    Grid &operator[](std::size_t i);

    VectorField<numCoords> &operator+=(const VectorField<numCoords> &rhs);
    VectorField<numCoords> &operator-=(const VectorField<numCoords> &rhs);
    VectorField<numCoords> &operator*=(Scalar rhs);

private:
    std::array<Grid, numCoords> grids;

};

template<std::size_t numCoords>
VectorField<numCoords> operator-=(VectorField<numCoords> lhs,
                                  const VectorField<numCoords> &rhs);
template<std::size_t numCoords>
VectorField<numCoords> operator-(VectorField<numCoords> lhs,
                                 const VectorField<numCoords> &rhs);
template<std::size_t numCoords>
VectorField<numCoords> operator*(VectorField<numCoords> lhs, Scalar rhs);

#include "vectorfield.tpp"

#endif // VECTORFIELD_H
