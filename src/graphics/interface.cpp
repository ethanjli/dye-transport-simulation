#include "interface.h"

#include <cmath>
#include <algorithm>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resourcemanager.h"

Interface::Interface(GLint width, GLint height, GLint depth, Scalar dt) :
    width(width), height(height), depth(depth), dt(dt),
    fluidSystem(std::make_shared<FluidSystem>(width, height, depth)),
    addDensity(fluidSystem->fullDim), addVelocity(fluidSystem->fullStaggeredDim) {
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
    ResourceManager::getShader("canvas").setInteger("width", width);
    ResourceManager::getShader("canvas").setInteger("height", height);
    ResourceManager::getShader("canvas").setInteger("depth", fluidSystem->dim(2));
    ResourceManager::getShader("canvas").setInteger("blurQuality", 3);
    ResourceManager::getShader("canvas").setFloat("saturation", 1);
    ResourceManager::getShader("canvas").setFloat("visibility", 0.8);
    ResourceManager::getShader("canvas").setTextureUnit("cyan", 0);
    ResourceManager::getShader("canvas").setTextureUnit("magenta", 1);
    ResourceManager::getShader("canvas").setTextureUnit("yellow", 2);
}

void Interface::update(GLfloat dt) {
    if (state != INTERFACE_ACTIVE) return;
    fluidSystem->step(addDensity, addVelocity, this->dt ? this->dt : dt);
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
