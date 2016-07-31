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
    Grid::Index centerX = 1 + fluidSystem->gridSize / 2;
    Grid::Index centerY = 1 + fluidSystem->gridSize / 2;
    Grid::Index initialWidth = fluidSystem->gridSize / 20;
    // Initialize velocities
    for (Grid::Index i = centerX - initialWidth / 10; i <= centerX + initialWidth / 10; ++i) {
        addVelocity[1](i, centerX + 4 * initialWidth) = -20;
    }
    // Initialize dyes
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
      for (Grid::Index j = centerY - initialWidth; j <= centerY + initialWidth; ++j) {
          fluidSystem->density[0](i,j) = 4;
      }
    }
    centerY = 1 + fluidSystem->gridSize / 4;
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
      for (Grid::Index j = centerY - initialWidth; j <= centerY + initialWidth; ++j) {
          fluidSystem->density[1](i,j) = 4;
      }
    }
    centerY = 1 + 3 * fluidSystem->gridSize / 4;
    for (Grid::Index i = centerX - initialWidth; i <= centerX + initialWidth; ++i) {
      for (Grid::Index j = centerY - initialWidth; j <= centerY + initialWidth; ++j) {
          fluidSystem->density[2](i,j) = 4;
      }
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
    ResourceManager::loadFluidTexture("fluid", fluidSystem);
    ResourceManager::getShader("canvas").setTextureUnit("cyan", 0);
    ResourceManager::getShader("canvas").setTextureUnit("magenta", 1);
    ResourceManager::getShader("canvas").setTextureUnit("yellow", 2);
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
