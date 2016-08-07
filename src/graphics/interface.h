#ifndef INTERFACE_H
#define INTERFACE_H

#include <array>
#include <GL/glew.h>

// Based on the LearnOpenGL 2D Game Rendering Sprites tutorial, with heavy modifications

#include "resourcemanager.h"
#include "canvas.h"

#include "fluidmanipulator.h"

enum SimulationState {
    INTERFACE_ACTIVE,
    INTERFACE_MENU,
    INTERFACE_PAUSED
};

class Interface
{
public:
    Interface(GLint width, GLint height, GLint depth, Scalar dt);
    ~Interface();

    SimulationState state = INTERFACE_PAUSED;
    std::array<GLboolean, 1024> keys = {{0}};
    std::array<GLboolean, 1024> keysUp = {{0}};
    std::array<GLboolean, 1024> keysDown = {{0}};
    std::array<GLboolean, 3> buttons = {{0}};
    std::array<GLboolean, 3> buttonsUp = {{0}};
    std::array<GLboolean, 3> buttonsDown = {{0}};
    std::array<double, 2> scroll = {{0}};

    // Initialize interface state (load all resources)
    void init();
    // Interface loop
    void processInput(GLfloat dt);
    void processResize(GLint width, GLint height);
    void update(GLfloat dt);
    void render();

private:
    Canvas *canvas;
    GLint width, height, depth;
    Scalar dt;
    std::shared_ptr<FluidSystem> fluidSystem;

    GLfloat saturation = 1;
    GLfloat visibility = 0.8;

    FluidManipulator manipulator;
};

#endif // INTERFACE_H
