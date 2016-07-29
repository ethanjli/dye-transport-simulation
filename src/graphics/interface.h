#ifndef INTERFACE_H
#define INTERFACE_H

#include <GL/glew.h>

// Based on the LearnOpenGL 2D Game Rendering Sprites tutorial, with heavy modifications

#include "resourcemanager.h"
#include "canvas.h"

#include "../fluid-sim/fluidsystem.h"

enum SimulationState {
    INTERFACE_ACTIVE,
    INTERFACE_MENU,
    INTERFACE_PAUSED
};

class Interface
{
public:
    Interface();
    ~Interface();

    SimulationState state = INTERFACE_ACTIVE;
    GLboolean keys[1024];

    // Initialize interface state (load all resources)
    void init(GLint width, GLint height);
    // Interface loop
    void processInput(GLfloat dt);
    void processResize(GLint width, GLint height);
    void update(GLfloat dt);
    void render();

private:
    Canvas *canvas;
    std::shared_ptr<FluidSystem> fluidSystem = std::make_shared<FluidSystem>();

    const Grid zero = Grid::Zero(kFullGridSize, kFullGridSize);
};

#endif // INTERFACE_H
