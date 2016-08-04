#include "interface.h"

#include <cmath>
#include <algorithm>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resourcemanager.h"

Interface::Interface(GLint width, GLint height) :
    width(width), height(height),
    fluidSystem(std::make_shared<FluidSystem>(width, height)),
    addDensity(fluidSystem->fullDim), addVelocity(fluidSystem->fullStaggeredDim) {
    Grid::Index centerX = fluidSystem->dim(0) / 2;
    Grid::Index centerY = fluidSystem->dim(1) / 2;
    Grid::Index initialWidth = fluidSystem->dim(0) / 8;
    Grid::Index initialHeight = fluidSystem->dim(1) / 8;
    initialWidth = std::min(initialWidth, initialHeight);
    initialHeight = std::min(initialWidth, initialHeight);
    // Initialize velocities
    Grid::Index offsetBase = initialWidth / 2;
    Grid::Index offsetSquare = initialWidth / 6;
    addVelocity[0](centerX - offsetSquare, centerY, 1) = -50;
    addVelocity[0](centerX + offsetSquare, centerY, 1) = 50;
    addVelocity[1](centerX, centerY - 3 * offsetBase - 2 * offsetSquare, 1) = 50;
    addVelocity[1](centerX, centerY - 3 * offsetBase + offsetSquare, 1) = 50;
    // Initialize dyes
    initialWidth = std::min(initialWidth, initialHeight);
    initialHeight = std::min(initialWidth, initialHeight);
    initialWidth /= 2;
    initialHeight /= 2;
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
      for (Grid::Index j = centerY - initialHeight; j <= centerY + initialHeight; ++j) {
          fluidSystem->density[1](i, j, 1) = 3;
          fluidSystem->density[1](i, j, 2) = 3;
      }
    }
    centerY = 1 + fluidSystem->dim(1) / 4;
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
      for (Grid::Index j = centerY - initialHeight; j <= centerY + initialHeight; ++j) {
          fluidSystem->density[1](i, j, 1) = 2;
          fluidSystem->density[1](i, j, 3) = 2;
      }
    }
    centerY = 1 + 3 * fluidSystem->dim(1) / 4;
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
      for (Grid::Index j = centerY - initialHeight; j <= centerY + initialHeight; ++j) {
          fluidSystem->density[1](i, j, 1) = 2;
          fluidSystem->density[1](i, j, 3) = 2;
      }
    }
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
        for (Grid::Index j = 1; j <= fluidSystem->dim(0); ++j) {
            fluidSystem->density[0](i, j, 4) = 2;
            fluidSystem->density[0](i, j, 4) = 2;
        }
    }
}

Interface::~Interface() {}

void Interface::init() {
    // Load shaders
    ResourceManager::loadShader("canvas", "shaders/canvas.vert", "shaders/canvas.frag").use();
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width),
                                      static_cast<GLfloat>(height), 0.0f,
                                      -1.0f, 1.0f);
    ResourceManager::getShader("canvas").setInteger("image", 0);
    ResourceManager::getShader("canvas").setMatrix4("projection", projection);
    // Set render-specific controls
    canvas = new Canvas(ResourceManager::getShader("canvas"), width, height);
    // Load textures
    ResourceManager::loadFluidTexture("fluid", fluidSystem);
    ResourceManager::getShader("canvas").setTextureUnit("cyan", 0);
    ResourceManager::getShader("canvas").setTextureUnit("magenta", 1);
    ResourceManager::getShader("canvas").setTextureUnit("yellow", 2);
}

void Interface::update(GLfloat dt) {
    if (state != INTERFACE_ACTIVE) return;
    fluidSystem->step(addDensity, addVelocity, dt);
    ResourceManager::getFluidTexture("fluid").update();
}

void Interface::processInput(GLfloat dt) {
    const GLfloat translateVelocity = std::min(width, height) / 4;
    const GLfloat rotateVelocity = 2;
    const GLfloat zoomVelocity = 4;
    const GLfloat maxZoom = 4;
    const GLfloat minZoom = 0.95;

    if (keys[GLFW_KEY_W]) { //pan move camera up with respect to canvas
        canvas->cameraX += translateVelocity * dt * std::sin(canvas->cameraAngle);
        canvas->cameraY += translateVelocity * dt * std::cos(canvas->cameraAngle);
    }
    if (keys[GLFW_KEY_S]) { // pan down with respect to canvas
        canvas->cameraX -= translateVelocity * dt * std::sin(canvas->cameraAngle);
        canvas->cameraY -= translateVelocity * dt * std::cos(canvas->cameraAngle);
    }
    if (keys[GLFW_KEY_D]) { // pan left with respect to canvas
        canvas->cameraX += translateVelocity * dt * std::sin(canvas->cameraAngle - 1.570796);
        canvas->cameraY += translateVelocity * dt * std::cos(canvas->cameraAngle - 1.570796);
    }
    if (keys[GLFW_KEY_A]) { // pan right with respect to canvas
        canvas->cameraX -= translateVelocity * dt * std::sin(canvas->cameraAngle - 1.570796);
        canvas->cameraY -= translateVelocity * dt * std::cos(canvas->cameraAngle - 1.570796);
    }
    if (keys[GLFW_KEY_Q]) { // rotate camera ccw with respect to canvas
        canvas->cameraAngle += rotateVelocity * dt;
    }
    if (keys[GLFW_KEY_E]) { // rotate camera cw with respect to canvas
        canvas->cameraAngle -= rotateVelocity * dt;
    }
    if (keys[GLFW_KEY_R]) { // zoom in
        canvas->cameraZoom = std::min(maxZoom, canvas->cameraZoom + zoomVelocity * dt);
    }
    if (keys[GLFW_KEY_F]) { // zoom out
        canvas->cameraZoom = std::max(minZoom, canvas->cameraZoom - zoomVelocity * dt);
    }

    if (keysUp[GLFW_KEY_SPACE]) { // toggle pause/unpause
        if (state == INTERFACE_ACTIVE) state = INTERFACE_PAUSED;
        else if (state == INTERFACE_PAUSED) state = INTERFACE_ACTIVE;

        keysUp[GLFW_KEY_SPACE] = GL_FALSE;
    }
}

void Interface::processResize(GLint newWidth, GLint newHeight) {
    width = newWidth;
    height = newHeight;
    canvas->width = newWidth;
    canvas->height = newHeight;
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(newWidth),
                                      static_cast<GLfloat>(newHeight), 0.0f,
                                      -1.0f, 1.0f);
    ResourceManager::getShader("canvas").setMatrix4("projection", projection);
}

void Interface::render() {
    canvas->draw(ResourceManager::getFluidTexture("fluid"));
}
