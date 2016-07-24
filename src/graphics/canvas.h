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
    Canvas(const Shader &shader, GLint width, GLint height);
    ~Canvas();

    GLint width;
    GLint height;
    GLfloat cameraAngle = 0;
    GLfloat cameraZoom = 1;
    GLfloat cameraX = 0;
    GLfloat cameraY = 0;

    // Renders a defined quad textured with a given sprite
    void draw(const Texture &texture);

private:
    Shader shader;
    GLuint quadVAO;

    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
};

#endif // CANVAS_H
