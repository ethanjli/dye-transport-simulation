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
    Interface(GLint width, GLint height, Grid::Index depth, Scalar dt);
    ~Interface();

    SimulationState state = INTERFACE_PAUSED;
    std::array<GLboolean, 1024> keys = {{0}};
    std::array<GLboolean, 1024> keysUp = {{0}};
    std::array<GLboolean, 1024> keysDown = {{0}};
    std::array<GLboolean, 3> buttons = {{0}};
    std::array<GLboolean, 3> buttonsUp = {{0}};
    std::array<GLboolean, 3> buttonsDown = {{0}};
    std::array<double, 2> scroll = {{0}};
    std::array<double, 2> cursor = {{0}};

    glm::vec4 viewport;

    // Initialize interface state (load all resources)
    void init();
    // Interface loop
    void processInput(GLfloat dt);
    void processResize(GLint width, GLint height, GLint windowZoom);
    void update(GLfloat dt);
    void render();

private:
    Canvas *canvas;
    glm::mat4 projectionMatrix;
    GLint width, height;
    Grid::Index depth;

    Scalar dt;
    std::shared_ptr<FluidSystem> fluidSystem;

    GLfloat saturation = 1;
    GLfloat visibility = 0.8;

    FluidManipulator manipulator;

    Scalar dropletCyan = 0;
    Scalar dropletMagenta = 0;
    Scalar dropletYellow = 0;
    Scalar dropletConcentration = 1;
    Grid::Index dropletRadius = 6;
    Grid::Index dropletDepth = 2;

    void processCameraInput(GLfloat dt);
    void processSimulationInput(GLfloat dt);
    void processRenderInput(GLfloat dt);
    void processManipulationInput(GLfloat dt);
};

#endif // INTERFACE_H
