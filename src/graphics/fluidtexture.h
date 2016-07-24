#ifndef FLUIDTEXTURE_H
#define FLUIDTEXTURE_H

#include <memory>

#include "../fluid-sim/fluidsystem.h"

// Based on the LearnOpenGL 2D Game Rendering Sprites tutorial

#include <GL/glew.h>

class FluidTexture
{
public:
    FluidTexture(const std::shared_ptr<FluidSystem> &fluidSystem);

    // Holds the ID of the fluid texture object
    GLuint id;
    // Fluid texture Format
    GLuint internalFormat = GL_RGB; // of fluid texture object
    GLuint imageFormat = GL_RGB; // of loaded image
    // Fluid texture configuration
    GLuint filterMin = GL_LINEAR; // if fluid texture pixels < screen pixels
    GLuint filterMax = GL_LINEAR; // if fluid texture pixels > screen pixels

    // Generates fluid texture from fluid data
    void generate();
    // Updates fluid texture from fluid data
    void update();
    // Binds the texture as the current active GL_TEXTURE_2D texture object
    void bind() const;

private:
    std::shared_ptr<FluidSystem> fluidSystem;
};

#endif // FLUIDTEXTURE_H
