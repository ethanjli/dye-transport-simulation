#include "interface.h"

#include <cmath>
#include <algorithm>
#include <iostream>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resourcemanager.h"

Interface::Interface(GLint width, GLint height, Grid::Index depth, Scalar dt) :
    viewport(0, 0, width, height), width(width), height(height), depth(depth),
    dt(dt), fluidSystem(std::make_shared<FluidSystem>(width, height, depth)),
    manipulator(fluidSystem) {}

Interface::~Interface() {}

void Interface::init() {
    // Load shaders
    ResourceManager::loadShader("canvas", "shaders/canvas.vert", "shaders/canvas.frag").use();
    // Configure shaders
    projectionMatrix = glm::ortho(0.0f, static_cast<GLfloat>(width),
                                  static_cast<GLfloat>(height), 0.0f, -1.0f, 1.0f);
    ResourceManager::getShader("canvas").setInteger("image", 0);
    ResourceManager::getShader("canvas").setMatrix4("projection", projectionMatrix);
    // Set render-specific controls
    canvas = new Canvas(ResourceManager::getShader("canvas"), width, height);
    // Load textures
    ResourceManager::loadFluidTexture("fluid", fluidSystem);
    ResourceManager::getShader("canvas").setInteger("width", width);
    ResourceManager::getShader("canvas").setInteger("height", height);
    ResourceManager::getShader("canvas").setInteger("scatterDepth", fluidSystem->dim(2) / 2);
    ResourceManager::getShader("canvas").setInteger("blur", 2);
    ResourceManager::getShader("canvas").setFloat("blurDiscreteness", 0.75);
    ResourceManager::getShader("canvas").setFloat("saturation", saturation);
    ResourceManager::getShader("canvas").setFloat("visibility", visibility);
    ResourceManager::getShader("canvas").setTextureUnit("cyan", 0);
    ResourceManager::getShader("canvas").setTextureUnit("magenta", 1);
    ResourceManager::getShader("canvas").setTextureUnit("yellow", 2);

    std::cout << "Droplet color is now CMY=(" << dropletCyan << ","
              << dropletMagenta << "," << dropletYellow << ")" << std::endl;
    std::cout << "Droplet concentration is now " << dropletConcentration << std::endl;
    std::cout << "Droplet depth is now " << dropletDepth << std::endl;
}

void Interface::update(GLfloat dt) {
    if (state != INTERFACE_ACTIVE) return;
    manipulator.step(this->dt ? this->dt : dt);
    ResourceManager::getFluidTexture("fluid").update();
}

void Interface::processInput(GLfloat dt) {
    processCameraInput(dt);
    processSimulationInput(dt);
    processRenderInput(dt);
    processManipulationInput(dt);
}

void Interface::processCameraInput(GLfloat dt) {
    const GLfloat translateVelocity = std::min(width, height) / 5;
    const GLfloat rotateVelocity = 0.5;
    const GLfloat zoomVelocity = 4;
    const GLfloat maxZoom = 3;
    const GLfloat minZoom = -0.05;

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
}

void Interface::processSimulationInput(GLfloat dt) {
    const GLfloat dtVelocity = 0.1;
    const GLfloat minDt = 0.01666667;
    const GLfloat maxDt = 0.1;

    if (keysUp[GLFW_KEY_SPACE]) { // toggle pause/unpause
        if (state == INTERFACE_ACTIVE) state = INTERFACE_PAUSED;
        else if (state == INTERFACE_PAUSED) state = INTERFACE_ACTIVE;

        keysUp[GLFW_KEY_SPACE] = GL_FALSE;
    }
    if (keys[GLFW_KEY_EQUAL]) { // speed up
        this->dt = std::min(maxDt, this->dt + dtVelocity * dt);
    }
    if (keys[GLFW_KEY_MINUS]) { // slow down
        this->dt = std::max(minDt, this->dt - dtVelocity * dt);
    }
    if (keysUp[GLFW_KEY_COMMA]) { // toggle horizontal boundary conditions
        fluidSystem->horizontalNeumann = !fluidSystem->horizontalNeumann;
        if (fluidSystem->horizontalNeumann) {
          std::cout << "Now using Neumann boundary conditions for the sides" << std::endl;
        } else {
            std::cout << "Now using continuity boundary conditions for the sides" << std::endl;
        }

        keysUp[GLFW_KEY_COMMA] = GL_FALSE;
    }
    if (keysUp[GLFW_KEY_PERIOD]) { // toggle vertical boundary conditions
        fluidSystem->verticalNeumann = !fluidSystem->verticalNeumann;
        if (fluidSystem->verticalNeumann) {
            std::cout << "Now using Neumann boundary conditions for the top and bottom" << std::endl;
        } else {
            std::cout << "Now using continuity boundary conditions for the top and bottom" << std::endl;
        }

        keysUp[GLFW_KEY_PERIOD] = GL_FALSE;
    }
}

void Interface::processRenderInput(GLfloat dt) {
    const GLfloat saturationVelocity = 1;
    const GLfloat minSaturation = 0.5;
    const GLfloat maxSaturation = 2;
    const GLfloat visibilityVelocity = 1;
    const GLfloat minVisibility = 0.2;
    const GLfloat maxVisibility = 1;

    if (scroll[1] > 0) {
        saturation = std::min(maxSaturation, saturation + saturationVelocity * dt);
        ResourceManager::getShader("canvas").setFloat("saturation", saturation);
        scroll[1] = 0;
    } else if (scroll[1] < 0) {
        saturation = std::max(minSaturation, saturation - saturationVelocity * dt);
        ResourceManager::getShader("canvas").setFloat("saturation", saturation);
        scroll[1] = 0;
    }
    if (scroll[0] > 0) {
        visibility = std::min(maxVisibility, visibility + visibilityVelocity * dt);
        ResourceManager::getShader("canvas").setFloat("visibility", visibility);
        scroll[0] = 0;
    } else if (scroll[0] < 0) {
        visibility = std::max(minVisibility, visibility - visibilityVelocity * dt);
        ResourceManager::getShader("canvas").setFloat("visibility", visibility);
        scroll[0] = 0;
    }
}

void Interface::processManipulationInput(GLfloat dt) {
    const Grid::Index dropletThickness = 2;
    if (keysUp[GLFW_KEY_APOSTROPHE]) {
        if (keys[GLFW_KEY_RIGHT_SHIFT] || keys[GLFW_KEY_LEFT_SHIFT]) {
            std::cout << "Clearing constant dye sources." << std::endl;
            manipulator.clearConstantDyeSource();
        } else {
            std::cout << "Clearing all dye." << std::endl;
            fluidSystem->density.clear();
        }
        keysUp[GLFW_KEY_APOSTROPHE] = GL_FALSE;
    }
    if (keysUp[GLFW_KEY_SLASH]) {
        if (keys[GLFW_KEY_RIGHT_SHIFT] || keys[GLFW_KEY_LEFT_SHIFT]) {
            std::cout << "Clearing constant flow sources." << std::endl;
            manipulator.clearConstantFlowSource();
        } else {
            std::cout << "Clearing all flow." << std::endl;
            fluidSystem->velocity.clear();
        }
        keysUp[GLFW_KEY_SLASH] = GL_FALSE;
    }
    // Adding dye
    if (keysUp[GLFW_KEY_Z]) {
        if (keys[GLFW_KEY_RIGHT_CONTROL] || keys[GLFW_KEY_LEFT_CONTROL]) {
            dropletCyan -= 0.25;
        } else {
            dropletCyan += 0.25;
        }
        dropletCyan = std::max(0.0f, std::min(1.0f, dropletCyan));
        std::cout << "Droplet color is now CMY=(" << dropletCyan << ","
                  << dropletMagenta << "," << dropletYellow << ")" << std::endl;

        keysUp[GLFW_KEY_Z] = GL_FALSE;
    }
    if (keysUp[GLFW_KEY_X]) {
        if (keys[GLFW_KEY_RIGHT_CONTROL] || keys[GLFW_KEY_LEFT_CONTROL]) {
            dropletMagenta -= 0.25;
        } else {
            dropletMagenta += 0.25;
        }
        dropletMagenta = std::max(0.0f, std::min(1.0f, dropletMagenta));
        std::cout << "Droplet color is now CMY=(" << dropletCyan << ","
                  << dropletMagenta << "," << dropletYellow << ")" << std::endl;

        keysUp[GLFW_KEY_X] = GL_FALSE;
    }
    if (keysUp[GLFW_KEY_C]) {
        if (keys[GLFW_KEY_RIGHT_CONTROL] || keys[GLFW_KEY_LEFT_CONTROL]) {
            dropletYellow -= 0.25;
        } else {
            dropletYellow += 0.25;
        }
        dropletYellow = std::max(0.0f, std::min(1.0f, dropletYellow));
        std::cout << "Droplet color is now CMY=(" << dropletCyan << ","
                  << dropletMagenta << "," << dropletYellow << ")" << std::endl;

        keysUp[GLFW_KEY_C] = GL_FALSE;
    }
    if (keysUp[GLFW_KEY_V]) {
        if (keys[GLFW_KEY_RIGHT_CONTROL] || keys[GLFW_KEY_LEFT_CONTROL]) {
            dropletConcentration -= 0.25;
        } else {
            dropletConcentration += 0.25;
        }
        dropletConcentration = std::max(0.0f, dropletConcentration);
        std::cout << "Droplet concentration is now " << dropletConcentration << std::endl;

        keysUp[GLFW_KEY_V] = GL_FALSE;
    }
    if (keysUp[GLFW_KEY_B]) {
        if (keys[GLFW_KEY_RIGHT_CONTROL] || keys[GLFW_KEY_LEFT_CONTROL]) {
            --dropletDepth;
        } else {
            ++dropletDepth;
        }
        dropletDepth = std::max(0L, std::min(depth, dropletDepth));
        std::cout << "Droplet depth is now " << dropletDepth << std::endl;

        keysUp[GLFW_KEY_B] = GL_FALSE;
    }
    if (buttonsUp[GLFW_MOUSE_BUTTON_LEFT]) {
        glm::vec3 cursorPos(cursor[0], cursor[1], 1.0f);
        glm::vec3 worldPos = glm::unProject(cursorPos, glm::mat4(), projectionMatrix, viewport);
        glm::vec4 gridPos = glm::vec4(worldPos, 1.0f);
        gridPos = canvas->viewInverseMatrix * gridPos;
        bool whetherConstant = keys[GLFW_KEY_RIGHT_SHIFT] || keys[GLFW_KEY_LEFT_SHIFT];
        manipulator.addDyeCircle(std::round(gridPos[0]), std::round(gridPos[1]),
                                 dropletRadius, dropletThickness,
                                 dropletCyan, dropletMagenta, dropletYellow,
                                 dropletConcentration, whetherConstant);
        buttonsUp[GLFW_MOUSE_BUTTON_LEFT] = GL_FALSE;
    }
}

void Interface::processResize(GLint newWidth, GLint newHeight, GLint windowZoom) {
    width = newWidth;
    height = newHeight;
    viewport = glm::vec4(0, 0, width * windowZoom, height * windowZoom);
    canvas->width = newWidth;
    canvas->height = newHeight;
    projectionMatrix = glm::ortho(0.0f, static_cast<GLfloat>(newWidth),
                                  static_cast<GLfloat>(newHeight), 0.0f,
                                  -1.0f, 1.0f);
    ResourceManager::getShader("canvas").setMatrix4("projection", projectionMatrix);
}

void Interface::render() {
    canvas->draw(ResourceManager::getFluidTexture("fluid"));
}
