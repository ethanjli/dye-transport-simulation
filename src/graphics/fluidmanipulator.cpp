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
               0, 1, 0, 0.16, true);
    addDyeCircle(centerX, centerY, halfLength * 4, 2,
                 1, 1, 0, 0.5);
}
void FluidManipulator::step(Scalar dt) {
    fluidSystem->step(constantDyeSource, constantFlowSource, dt);
}

void FluidManipulator::addDyeRect(int x, int y, int halfLength, int halfHeight,
                                  Grid::Index depthStart, Grid::Index depthStop,
                                  Scalar cyan, Scalar magenta, Scalar yellow,
                                  Scalar concentration, bool constantSource) {
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
                (*target)[0](i, j, k) = cyan * concentration;
                (*target)[1](i, j, k) = magenta * concentration;
                (*target)[2](i, j, k) = yellow * concentration;
            }
        }
    }
}
void FluidManipulator::addDyeCircle(int x, int y, int r, Grid::Index depthStop,
                                    Scalar cyan, Scalar magenta, Scalar yellow,
                                    Scalar concentration, bool constantSource) {
    DyeField *target;
    if (constantSource) {
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
            // Is this how real people do antialiasing? Who knows!
            // But this heuristic works well, and it makes sense (because implicit functions)
            Scalar antialias = 1;
            if (outerDistance >= -2.0 * r) {
                antialias = -outerDistance / (2.0 * r);
                if (concentration >= 1) antialias /= concentration;
                else antialias *= concentration;
            }
            for (Grid::Index k = 1; k <= depthStop; ++k) {
                (*target)[0](i, j, k) = cyan * concentration * antialias;
                (*target)[1](i, j, k) = magenta * concentration * antialias;
                (*target)[2](i, j, k) = yellow * concentration * antialias;
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
