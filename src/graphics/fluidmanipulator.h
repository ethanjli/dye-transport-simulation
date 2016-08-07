#ifndef FLUIDMANIPULATOR_H
#define FLUIDMANIPULATOR_H

#include <memory>

#include "../fluid-sim/fluidsystem.h"

class FluidManipulator
{
public:
    FluidManipulator(std::shared_ptr<FluidSystem> fluidSystem);

    void step(Scalar dt);

    void addDyeRect(Scalar x, Scalar y, Scalar halfLength, Scalar halfHeight,
                    Scalar depthStart, Scalar depthStop,
                    Scalar cyan, Scalar magenta, Scalar yellow,
                    bool constantSource = false);

    void clearConstantDyeSource();
    void clearConstantFlowSource();

private:
    std::shared_ptr<FluidSystem> fluidSystem;
    DyeField constantDyeSource;
    VelocityField constantFlowSource;
};

#endif // FLUIDMANIPULATOR_H
