#ifndef FLUIDMANIPULATOR_H
#define FLUIDMANIPULATOR_H

#include <memory>

#include "../fluid-sim/fluidsystem.h"

class FluidManipulator
{
public:
    FluidManipulator(std::shared_ptr<FluidSystem> fluidSystem);

    void step(Scalar dt);

private:
    std::shared_ptr<FluidSystem> fluidSystem;
    DyeField addDensity;
    VelocityField addVelocity;
};

#endif // FLUIDMANIPULATOR_H
