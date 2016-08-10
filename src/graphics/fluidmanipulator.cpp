#include "fluidmanipulator.h"

#include <iostream>

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
    addSoapRect(centerX, centerY, 5, 5, 2000, 2000, kAdditionConstantAdditive);
    // Initialize dyes
    Scalar halfLength = std::min(initialWidth, initialHeight) / 2;
    //addDyeRect(centerX, 1, fluidSystem->dim(0) / 3, 1, 4, 6,
    //           0, 1, 0, 3.5, kAdditionAdditive);
    addDyeCircle(centerX, centerY, halfLength * 4, 2, 1, 1, 0, 0.5);
}
void FluidManipulator::step(Scalar dt) {
    fluidSystem->step(constantDyeSource, constantFlowSource, dt);
}

void FluidManipulator::addDyeRect(int x, int y, int halfLength, int halfHeight,
                                  Grid::Index depthStart, Grid::Index depthStop,
                                  Scalar cyan, Scalar magenta, Scalar yellow,
                                  Scalar concentration, AdditionMode mode) {
    DyeField *target;
    if (mode == kAdditionConstantAdditive) {
        target = &constantDyeSource;
    } else {
        target = &(fluidSystem->density);
    }
    for (Grid::Index i = x - halfLength; i <= x + halfLength; ++i) {
        if (i < 0 || i > fluidSystem->dim(0)) continue;
        for (Grid::Index j = y - halfHeight; j <= y + halfHeight; ++j) {
            if (j < 0 || j > fluidSystem->dim(1)) continue;
            for (Grid::Index k = depthStart; k <= depthStop; ++k) {
                (*target)[0](i, j, k) = cyan * concentration;
                (*target)[1](i, j, k) = magenta * concentration;
                (*target)[2](i, j, k) = yellow * concentration;
            }
        }
    }
}
void FluidManipulator::addDyeCircle(int x, int y, int r, Grid::Index depthStop,
                                    Scalar cyan, Scalar magenta, Scalar yellow,
                                    Scalar concentration, AdditionMode mode) {
    DyeField *target;
    if (mode == kAdditionConstantAdditive) {
        target = &constantDyeSource;
    } else {
        target = &(fluidSystem->density);
    }
    for (int i = x - r; i <= x + r; ++i) {
        if (i < 0 || i > fluidSystem->dim(0)) continue;
        for (int j = y - r; j <= y + r; ++j) {
            if (j < 0 || j > fluidSystem->dim(1)) continue;
            int dx = i - x;
            int dy = j - y;
            int outerDistance = dx * dx + dy * dy - r * r;
            if (outerDistance > 0) continue;
            // Is this how real people do antialiasing? Who knows! I hacked this
            // together by trial and error (and by applying the coverage of implicit
            // functions from the first lecture). This heuristic works well, so.
            Scalar antialias = 1;
            if (outerDistance >= -2.0 * r) {
                antialias = -outerDistance / (2.0 * r);
                if (concentration >= 1) antialias /= concentration;
                else antialias *= concentration;
            }
            for (Grid::Index k = 1; k <= depthStop; ++k) {
                if (mode == kAdditionReplacement) {
                    (*target)[0](i, j, k) = 0;
                    (*target)[1](i, j, k) = 0;
                    (*target)[2](i, j, k) = 0;
                }
                (*target)[0](i, j, k) += cyan * concentration * antialias;
                (*target)[1](i, j, k) += magenta * concentration * antialias;
                (*target)[2](i, j, k) += yellow * concentration * antialias;
            }
        }
    }
}
void FluidManipulator::addSoapRect(int x, int y, int halfLength, int halfHeight,
                                   Scalar outwardsVelocity, Scalar upwardsVelocity,
                                   AdditionMode mode) {
    VelocityField *target;
    if (mode == kAdditionConstantAdditive) {
        target = &constantFlowSource;
    } else {
        target = &(fluidSystem->velocity);
    }
    // Add top and bottom velocities
    for (Grid::Index i = x - halfLength; i <= x + halfLength; ++i) {
        if (i < 0 || i > fluidSystem->dim(0)) continue;
        if (y - halfHeight >= 0) { // bottom in bounds
            (*target)[1](i, y - halfHeight, 1) = -outwardsVelocity;
            (*target)[2](i, y - halfHeight, 1) = -upwardsVelocity;
        }
        if (y + halfHeight <= fluidSystem->dim(1)) { // top in bounds
            (*target)[1](i, y + halfHeight, 1) = outwardsVelocity;
            (*target)[2](i, y + halfHeight, 1) = -upwardsVelocity;
        }
    }
    // Add left and right velocities
    for (Grid::Index j = y - halfHeight; j <= y + halfHeight; ++j) {
        if (j < 0 || j > fluidSystem->dim(1)) continue;
        if (x - halfLength >= 0) { // left in bounds
            (*target)[0](x - halfLength, j, 1) = -outwardsVelocity;
            (*target)[2](x - halfLength, j, 1) = -upwardsVelocity;
        }
        if (x + halfLength <= fluidSystem->dim(0)) { // right in bounds
            (*target)[0](x + halfLength, j, 1) = outwardsVelocity;
            (*target)[2](x + halfLength, j, 1) = -upwardsVelocity;
        }
    }
}


void FluidManipulator::clearConstantDyeSource() {
    constantDyeSource.clear();
}
void FluidManipulator::clearConstantFlowSource() {
    constantFlowSource.clear();
}
