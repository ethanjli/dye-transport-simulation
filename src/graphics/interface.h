#ifndef INTERFACE_H
#define INTERFACE_H

#include <GL/glew.h>

// Based on the LearnOpenGL 2D Game Rendering Sprites tutorial

#include "resourcemanager.h"
#include "canvas.h"

enum SimulationState {
    INTERFACE_ACTIVE,
    INTERFACE_MENU,
    INTERFACE_PAUSED
};

class Interface
{
public:
    Interface(GLuint width, GLuint height);
    ~Interface();

    SimulationState state = INTERFACE_ACTIVE;
    GLboolean keys[1024];
    GLuint width, height;

    // Initialize interface state (load all resources)
    void init();
    // Interface loop
    void processInput(GLfloat dt);
    void update(GLfloat dt);
    void render();

private:
    Canvas *canvas;
};

#endif // INTERFACE_H
