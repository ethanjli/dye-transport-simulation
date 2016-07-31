#ifndef FLUIDTEXTURE_H
#define FLUIDTEXTURE_H

#include <memory>
#include <array>

#include <GL/glew.h>

#include "../fluid-sim/fluidsystem.h"

// Based on the LearnOpenGL 2D Game Rendering Sprites tutorial, with heavy modifications

class FluidTexture
{
public:
    FluidTexture(const std::shared_ptr<FluidSystem> &fluidSystem);

    static const std::size_t textures = DyeField::coords;

    // Holds the ID of the fluid texture object
    std::array<GLuint, textures> ids;
    // Fluid texture Format
    GLuint internalFormat = GL_RED; // of fluid texture object for each color channel
    GLuint format = GL_RED; // of loaded image for each color channel
    // Fluid texture configuration
    GLuint filterMin = GL_LINEAR; // if fluid texture pixels < screen pixels
    GLuint filterMax = GL_LINEAR; // if fluid texture pixels > screen pixels

    // Generates fluid texture from fluid data
    void generate();
    // Updates fluid texture from fluid data
    void update();
    // Binds the specified color channel as the current active GL_TEXTURE_2D texture object
    void bind(std::size_t channel) const;

private:
    std::shared_ptr<FluidSystem> fluidSystem;
};

#endif // FLUIDTEXTURE_H
