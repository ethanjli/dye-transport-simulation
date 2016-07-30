#ifndef MATH_H
#define MATH_H

#include <array>

#include <Eigen/Dense>

typedef float Scalar;
typedef Eigen::Array<Scalar, Eigen::Dynamic, Eigen::Dynamic> Grid;

template<std::size_t numCoords>
class VectorField {
public:
    VectorField(Grid::Index numRows, Grid::Index numCols);

    void clear();

    const Grid &operator[](std::size_t i) const;
    Grid &operator[](std::size_t i);

    VectorField<numCoords> &operator+=(const VectorField<numCoords> &rhs);
    VectorField<numCoords> &operator-=(const VectorField<numCoords> &rhs);

private:
    std::array<Grid, numCoords> grids;

};

template<std::size_t numCoords>
VectorField<numCoords> operator-=(VectorField<numCoords> lhs,
                                  const VectorField<numCoords> &rhs);

template<std::size_t numCoords>
VectorField<numCoords> operator-(VectorField<numCoords> lhs,
                                 const VectorField<numCoords> &rhs);

#include "math.tpp"

#endif // MATH_H
