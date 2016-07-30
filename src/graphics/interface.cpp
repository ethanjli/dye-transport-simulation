#include "interface.h"

#include <cmath>
#include <algorithm>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resourcemanager.h"

Interface::Interface() :
    keys(), addDensity(fluidSystem->fullGridSize, fluidSystem->fullGridSize),
    addVelocity(fluidSystem->fullGridSize, fluidSystem->fullGridSize) {
    Grid::Index center = 1 + fluidSystem->gridSize / 2;
    Grid::Index initialWidth = 20;
    for (Grid::Index i = center - initialWidth; i <= center + initialWidth; ++i) {
      for (Grid::Index j = center - initialWidth; j <= center + initialWidth; ++j) {
          fluidSystem->density[0](i,j) = 4;
      }
    }
    for (Grid::Index i = center - initialWidth / 10; i <= center + initialWidth / 10; ++i) {
        addVelocity[1](i, center + 4 * initialWidth) = -1;
    }
}

Interface::~Interface() {}

void Interface::init(GLint width, GLint height) {
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
    ResourceManager::loadFluidTexture("fluid", fluidSystem, GL_FALSE);
}

void Interface::update(GLfloat dt) {
    fluidSystem->step(addDensity, addVelocity, dt);
    ResourceManager::getFluidTexture("fluid").update();
}

void Interface::processInput(GLfloat dt) {
    const GLfloat translateVelocity = 100;
    const GLfloat rotateVelocity = 2;
    const GLfloat zoomVelocity = 4;
    const GLfloat maxZoom = 4;
    const GLfloat minZoom = 1;

    if (keys[GLFW_KEY_W]) {
        canvas->cameraX += translateVelocity * dt * std::sin(canvas->cameraAngle);
        canvas->cameraY += translateVelocity * dt * std::cos(canvas->cameraAngle);
    }
    if (keys[GLFW_KEY_S]) {
        canvas->cameraX -= translateVelocity * dt * std::sin(canvas->cameraAngle);
        canvas->cameraY -= translateVelocity * dt * std::cos(canvas->cameraAngle);
    }
    if (keys[GLFW_KEY_D]) {
        canvas->cameraX += translateVelocity * dt * std::sin(canvas->cameraAngle - 1.570796);
        canvas->cameraY += translateVelocity * dt * std::cos(canvas->cameraAngle - 1.570796);
    }
    if (keys[GLFW_KEY_A]) {
        canvas->cameraX -= translateVelocity * dt * std::sin(canvas->cameraAngle - 1.570796);
        canvas->cameraY -= translateVelocity * dt * std::cos(canvas->cameraAngle - 1.570796);
    }
    if (keys[GLFW_KEY_Q]) {
        canvas->cameraAngle += rotateVelocity * dt;
    }
    if (keys[GLFW_KEY_E]) {
        canvas->cameraAngle -= rotateVelocity * dt;
    }
    if (keys[GLFW_KEY_R]) {
        canvas->cameraZoom = std::min(maxZoom, canvas->cameraZoom + zoomVelocity * dt);
    }
    if (keys[GLFW_KEY_F]) {
        canvas->cameraZoom = std::max(minZoom, canvas->cameraZoom - zoomVelocity * dt);
    }
}

void Interface::processResize(GLint width, GLint height) {
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width),
                                      static_cast<GLfloat>(height), 0.0f,
                                      -1.0f, 1.0f);
    ResourceManager::getShader("canvas").setMatrix4("projection", projection);
}

void Interface::render() {
    canvas->draw(ResourceManager::getFluidTexture("fluid"));
}
