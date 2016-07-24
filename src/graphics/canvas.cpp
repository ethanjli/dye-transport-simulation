#include "canvas.h"

#include <glm/gtc/matrix_transform.hpp>

Canvas::Canvas(const Shader &shader) :
    shader(shader)
{
    initRenderData();
}
Canvas::~Canvas() {
    glDeleteVertexArrays(1, &quadVAO);
}

void Canvas::draw(const Texture &texture, glm::vec2 position, glm::vec2 size, GLfloat rotate) {
    shader.use();
    glm::mat4 view;
    // Move
    view = glm::translate(view, glm::vec3(position, 0.0f));
    // Rotate
    view = glm::translate(view, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    view = glm::rotate(view, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::translate(view, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    // Scale
    view = glm::scale(view, glm::vec3(size, 1.0f));
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
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
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
