#ifndef SHADER_H
#define SHADER_H

// Based on the LearnOpenGL 2D Game Rendering Sprites tutorial

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

// General purpose shader object. Compiles from file, generates compile/link-time
// error messages, and hosts utility functions.
class Shader
{
public:
    Shader() {}

    GLuint id;

    // Sets the current shader as active
    Shader &use();

    // Compiles the shader from given source code
    void compile(const std::string &vertexSoruce, const std::string &fragmentSource,
                 const std::string &geometrySource);

    // Utility functions
    void setFloat(const GLchar *name, GLfloat value, GLboolean useShader = false);
    void setInteger(const GLchar *name, GLint value, GLboolean useShader = false);
    void setVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = false);
    void setVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader = false);
    void setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = false);
    void setVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader = false);
    void setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = false);
    void setVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader = false);
    void setMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = false);

private:
    //checks if compilation or linking failed, and if so, prints to stderr
    void checkCompileErrors(GLuint object, std::string type);
};

#endif
