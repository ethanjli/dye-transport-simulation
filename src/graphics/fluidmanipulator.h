#ifndef FLUIDMANIPULATOR_H
#define FLUIDMANIPULATOR_H

#include <memory>

#include "../fluid-sim/fluidsystem.h"

enum AdditionMode {
    kAdditionReplacement,
    kAdditionAdditive,
    kAdditionConstantAdditive
};

class FluidManipulator
{
public:
    FluidManipulator(std::shared_ptr<FluidSystem> fluidSystem);

    void step(Scalar dt);

    void addDyeRect(int x, int y, int halfLength, int halfHeight,
                    Grid::Index depthStart, Grid::Index depthStop,
                    Scalar cyan, Scalar magenta, Scalar yellow,
                    Scalar concentration = 1, AdditionMode mode = kAdditionAdditive);
    void addDyeCircle(int x, int y, int radius, Grid::Index depthStop,
                      Scalar cyan, Scalar magenta, Scalar yellow,
                      Scalar concentration = 1,
                      AdditionMode mode = kAdditionAdditive);
    void addSoapRect(int x, int y, int halfLength, int halfHeight,
                     Scalar outwardsVelocity, Scalar upwardsVelocity,
                     AdditionMode mode = kAdditionAdditive);
    /*
    void addSoapCircle(int x, int y, int radius, Grid::Index depthStop,
                       AdditionMode mode = kAdditionAdditive);
                       */

    void clearConstantDyeSource();
    void clearConstantFlowSource();

private:
    std::shared_ptr<FluidSystem> fluidSystem;
    DyeField constantDyeSource;
    VelocityField constantFlowSource;
};

#endif // FLUIDMANIPULATOR_H
