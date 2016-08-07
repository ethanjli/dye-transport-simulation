#include "canvas.h"

#include <glm/gtc/matrix_transform.hpp>

Canvas::Canvas(const Shader &shader, GLint width, GLint height) :
    width(width), height(height), shader(shader)
{
    initRenderData();
}
Canvas::~Canvas() {
    glDeleteVertexArrays(1, &quadVAO);
}

void Canvas::draw(const FluidTexture &fluidTexture) {
    const GLfloat rotate = cameraAngle;

    shader.use();

    glm::mat4 view, viewInverse;

    // Canvas to pre-projection
    view = glm::translate(view, glm::vec3(0.5 * width, 0.5 * height, 0.0f));
    view = glm::scale(view, glm::vec3(1 + cameraZoom, 1 + cameraZoom, 1.0f));
    view = glm::rotate(view, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    view = glm::translate(view, glm::vec3(cameraX, cameraY, 0.0f));
    // Post-unprojection to canvas
    viewInverse = glm::translate(viewInverse, glm::vec3(-cameraX, cameraY, 0.0f));
    viewInverse = glm::rotate(viewInverse, rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    viewInverse = glm::scale(viewInverse, glm::vec3(1 / (1 + cameraZoom), 1 / (1 + cameraZoom), 1.0f));
    viewInverse = glm::translate(viewInverse, glm::vec3(-0.5 * width, -0.5 * height, 0.0f));

    shader.setMatrix4("view", view);

    viewMatrix = view;
    viewInverseMatrix = viewInverse;

    for (std::size_t i = 0; i < fluidTexture.textures; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        fluidTexture.bind(i);
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Canvas::initRenderData() {
    GLuint VBO;
    GLfloat vertices[] = {
        // Pos                         // Tex
        -0.5f * width, 0.5f * height,  0.0f, 0.0f,
        0.5f * width, -0.5f * height,  1.0f, 1.0f,
        -0.5f * width, -0.5f * height, 0.0f, 1.0f,

        -0.5f * width, 0.5f * height,  0.0f, 0.0f,
        0.5f * width, 0.5f * height,   1.0f, 0.0f,
        0.5f * width, -0.5f * height,  1.0f, 1.0f
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
