#ifndef CANVAS_H
#define CANVAS_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

// Based on the LearnOpenGL 2D Game Rendering Sprites tutorial

class Canvas
{
public:
    Canvas(const Shader &shader);
    ~Canvas();

    // Renders a defined quad textured with a given sprite
    void draw(const Texture &texture, glm::vec2 position, glm::vec2 size, GLfloat rotate);

private:
    Shader shader;
    GLuint quadVAO;

    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};

#endif // CANVAS_H
