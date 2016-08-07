#include "fluidmanipulator.h"

FluidManipulator::FluidManipulator(std::shared_ptr<FluidSystem> fluidSystem) :
    fluidSystem(fluidSystem), addDensity(fluidSystem->fullDim),
    addVelocity(fluidSystem->fullStaggeredDim) {
    Grid::Index centerX = fluidSystem->dim(0) / 2;
    Grid::Index centerY = fluidSystem->dim(1) / 2;
    Grid::Index initialWidth = fluidSystem->dim(0) / 8;
    Grid::Index initialHeight = fluidSystem->dim(1) / 8;
    initialWidth = std::min(initialWidth, initialHeight);
    initialHeight = std::min(initialWidth, initialHeight);
    // Initialize velocities
    addVelocity[0](centerX - 5, centerY, 1) = -100;
    addVelocity[0](centerX + 5, centerY, 1) = 100;
    addVelocity[1](centerX, centerY - 5, 1) = 100;
    addVelocity[1](centerX, centerY + 5, 1) = 100;
    for (Grid::Index i = centerX - 3; i <= centerX + 3; ++i) {
        for (Grid::Index j = centerY - 3; j <= centerY + 3; ++j) {
            addVelocity[2](i, j, 2) = 40;
        }
    }
    // Initialize dyes
    Scalar concentrationGain = 4;
    initialWidth = std::min(initialWidth, initialHeight);
    initialHeight = std::min(initialWidth, initialHeight);
    initialWidth /= 2;
    initialHeight /= 2;
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
      for (Grid::Index j = centerY - initialHeight; j <= centerY + initialHeight; ++j) {
          fluidSystem->density[0](i, j, 1) = 0.25;
          fluidSystem->density[0](i, j, 1) = 0.25;
          fluidSystem->density[1](i, j, 1) = 1;
          fluidSystem->density[1](i, j, 1) = 1;
          fluidSystem->density[0](i, j, 2) = 0.25;
          fluidSystem->density[0](i, j, 2) = 0.25;
          fluidSystem->density[1](i, j, 1) = 1;
          fluidSystem->density[1](i, j, 1) = 1;
          //fluidSystem->density[0](i, j, 1) = 1;
      }
    }
    centerY = 1 + fluidSystem->dim(1) / 4;
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
      for (Grid::Index j = centerY - initialHeight; j <= centerY + initialHeight; ++j) {
          fluidSystem->density[1](i, j, 1) = 1;
          fluidSystem->density[1](i, j, 1) = 1;
          fluidSystem->density[1](i, j, 2) = 0.5;
          fluidSystem->density[1](i, j, 2) = 0.5;
          //fluidSystem->density[1](i, j, 2) = 1;
      }
    }
    centerY = 1 + 3 * fluidSystem->dim(1) / 4;
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
      for (Grid::Index j = centerY - initialHeight; j <= centerY + initialHeight; ++j) {
          fluidSystem->density[1](i, j, 1) = 1;
          fluidSystem->density[1](i, j, 1) = 1;
          fluidSystem->density[1](i, j, 2) = 0.5;
          fluidSystem->density[1](i, j, 2) = 0.5;
          //fluidSystem->density[2](i, j, 3) = 1;
      }
    }
    fluidSystem->density *= concentrationGain;
}

void FluidManipulator::step(Scalar dt) {
    fluidSystem->step(addDensity, addVelocity, dt);
}
