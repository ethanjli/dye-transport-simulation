#include "fluidmanipulator.h"

FluidManipulator::FluidManipulator(std::shared_ptr<FluidSystem> fluidSystem) :
    fluidSystem(fluidSystem), constantDyeSource(fluidSystem->fullDim),
    constantFlowSource(fluidSystem->fullStaggeredDim) {
    Grid::Index centerX = fluidSystem->dim(0) / 2;
    Grid::Index centerY = fluidSystem->dim(1) / 2;
    Grid::Index initialWidth = fluidSystem->dim(0) / 8;
    Grid::Index initialHeight = fluidSystem->dim(1) / 8;
    initialWidth = std::min(initialWidth, initialHeight);
    initialHeight = std::min(initialWidth, initialHeight);
    // Initialize velocities
    constantFlowSource[0](centerX - 5, centerY, 1) = -100;
    constantFlowSource[0](centerX + 5, centerY, 1) = 100;
    constantFlowSource[1](centerX, centerY - 5, 1) = 100;
    constantFlowSource[1](centerX, centerY + 5, 1) = 100;
    for (Grid::Index i = centerX - 3; i <= centerX + 3; ++i) {
        for (Grid::Index j = centerY - 3; j <= centerY + 3; ++j) {
            constantFlowSource[2](i, j, 2) = 40;
        }
    }
    // Initialize dyes
    Scalar halfLength = std::min(initialWidth, initialHeight) / 2;
    addDyeRect(centerX, 1, fluidSystem->dim(0) / 3, 1, 4, 6,
               0, 0.04, 0, true);
    addDyeRect(centerX, centerY, halfLength, halfLength, 1, 2,
               0.25, 1, 0);
    addDyeRect(centerX, 0.75 * fluidSystem->dim(1), halfLength, halfLength, 1, 2,
               0, 1, 0);

    Scalar concentrationGain = 4;
    fluidSystem->density *= concentrationGain;
}

void FluidManipulator::step(Scalar dt) {
    fluidSystem->step(constantDyeSource, constantFlowSource, dt);
}

void FluidManipulator::addDyeRect(Scalar x, Scalar y,
                                  Scalar halfLength, Scalar halfHeight,
                                  Scalar depthStart, Scalar depthStop,
                                  Scalar cyan, Scalar magenta, Scalar yellow,
                                  bool constantSource) {
    DyeField *target;
    if (constantSource) {
        target = &constantDyeSource;
    } else {
        target = &(fluidSystem->density);
    }
    for (Grid::Index i = x - halfLength; i <= x + halfLength; ++i) {
        if (i < 0 || i > fluidSystem->dim(0)) continue;
        for (Grid::Index j = y - halfHeight; j <= y + halfHeight; ++j) {
            if (j < 0 || j > fluidSystem->dim(1)) continue;
            for (Grid::Index k = depthStart; k <= depthStop; ++k) {
                (*target)[0](i, j, k) = cyan;
                (*target)[1](i, j, k) = magenta;
                (*target)[2](i, j, k) = yellow;
            }
        }
    }
}

void FluidManipulator::clearConstantDyeSource() {
    constantDyeSource.clear();
}
void FluidManipulator::clearConstantFlowSource() {
    constantFlowSource.clear();
}
