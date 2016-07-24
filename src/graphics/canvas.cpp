#include "canvas.h"

#include <glm/gtc/matrix_transform.hpp>

Canvas::Canvas(const Shader &shader, GLint width, GLint height) :
    shader(shader), width(width), height(height)
{
    initRenderData();
}
Canvas::~Canvas() {
    glDeleteVertexArrays(1, &quadVAO);
}

void Canvas::draw(const Texture &texture) {
    const GLfloat rotate = cameraAngle;

    shader.use();

    glm::mat4 view;

    view = glm::translate(view, glm::vec3(0.5 * width, 0.5 * height, 0.0f));
    // Scale
    view = glm::scale(view, glm::vec3(cameraZoom, cameraZoom, 1.0f));
    // Rotate
    view = glm::rotate(view, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    // Move
    view = glm::translate(view, glm::vec3(cameraX, cameraY, 0.0f));
    // Scale to the frame's size
    view = glm::scale(view, glm::vec3(height, width, 1.0f));

    shader.setMatrix4("view", view);

    glActiveTexture(GL_TEXTURE0);
    texture.bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Canvas::initRenderData() {
    GLuint VBO;
    GLfloat vertices[] = {
        // Pos      // Tex
        -0.5f, 0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
