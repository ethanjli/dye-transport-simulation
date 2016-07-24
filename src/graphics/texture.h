#ifndef TEXTURE_H
#define TEXTURE_H

// Based on the LearnOpenGL 2D Game Rendering Sprites tutorial

#include <GL/glew.h>

class Texture
{
public:
    Texture();

    // Holds the ID of the texture object
    GLuint id;
    // Texture image dimensions
    GLuint width = 0;
    GLuint height = 0; // of the loaded image in pixels
    // Texture Format
    GLuint internalFormat = GL_RGB; // of texture object
    GLuint imageFormat = GL_RGB; // of loaded image
    // Texture configuration
    GLuint filterMin = GL_LINEAR; // if texture pixels < screen pixels
    GLuint filterMax = GL_LINEAR; // if texture pixels > screen pixels

    // Generates texture from image data
    void generate(GLuint width, GLuint height, unsigned char *data);
    // Binds the texture as the current active GL_TEXTURE_2D texture object
    void bind() const;
};

#endif // TEXTURE_H
