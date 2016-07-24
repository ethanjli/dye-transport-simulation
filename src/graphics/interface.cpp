#include "interface.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resourcemanager.h"

Interface::Interface(GLuint width, GLuint height) :
    keys(), width(width), height(height) {}

Interface::~Interface() {}

void Interface::init() {
    // Load shaders
    ResourceManager::loadShader("canvas", "shaders/canvas.vert", "shaders/canvas.frag");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(width),
                                      static_cast<GLfloat>(height), 0.0f,
                                      -1.0f, 1.0f);
    ResourceManager::getShader("canvas").use().setInteger("image", 0);
    ResourceManager::getShader("canvas").setMatrix4("projection", projection);
    // Set render-specific controls
    canvas = new Canvas(ResourceManager::getShader("canvas"));
    // Load textures
    ResourceManager::loadTexture("face", "textures/awesomeface.png", GL_TRUE);
}

void Interface::update(GLfloat dt) {

}

void Interface::processInput(GLfloat dt) {

}

void Interface::render() {
    canvas->draw(ResourceManager::getTexture("face"), glm::vec2(200, 200),
                 glm::vec2(300, 400), 45.0f);
}
