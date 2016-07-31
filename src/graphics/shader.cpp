#include "shader.h"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

Shader &Shader::use() {
    glUseProgram(id);
    return *this;
}

void Shader::compile(const std::string &vertexSource, const std::string &fragmentSource,
                     const std::string &geometrySource) {
    const GLchar *vShaderCode = vertexSource.c_str();
    const GLchar *fShaderCode = fragmentSource.c_str();
    const GLchar *gShaderCode = geometrySource.size() ? geometrySource.c_str() : nullptr;
    // Vertex Shader
    GLuint sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vShaderCode, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    // Fragment Shader
    GLuint sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fShaderCode, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");
    // Geometry Shader
    GLuint gShader;
    if (geometrySource.size()) {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &gShaderCode, NULL);
        glCompileShader(sFragment);
        checkCompileErrors(gShader, "GEOMETRY");
    }
    // Shader Program
    id = glCreateProgram();
    glAttachShader(id, sVertex);
    glAttachShader(id, sFragment);
    if (geometrySource.size()) glAttachShader(id, gShader);
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");
    // Clean Up
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource.size()) glDeleteShader(gShader);
}

void Shader::setTextureUnit(const GLchar *name, GLint textureUnit, GLboolean useShader) {
    if (useShader) use();
    glUniform1i(glGetUniformLocation(id, name), textureUnit);
}
void Shader::setFloat(const GLchar *name, GLfloat value, GLboolean useShader) {
    if (useShader) use();
    glUniform1f(glGetUniformLocation(id, name), value);
}
void Shader::setInteger(const GLchar *name, GLint value, GLboolean useShader) {
    if (useShader) use();
    glUniform1i(glGetUniformLocation(id, name), value);
}
void Shader::setVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader) {
    if (useShader) use();
    glUniform2f(glGetUniformLocation(id, name), x, y);
}
void Shader::setVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader) {
    if (useShader) use();
    glUniform2f(glGetUniformLocation(id, name), value.x, value.y);
}
void Shader::setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader) {
    if (useShader) use();
    glUniform3f(glGetUniformLocation(id, name), x, y, z);
}
void Shader::setVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader) {
    if (useShader) use();
    glUniform3f(glGetUniformLocation(id, name), value.x, value.y, value.z);
}
void Shader::setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader) {
    if (useShader) use();
    glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}
void Shader::setVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader) {
    if (useShader) use();
    glUniform4f(glGetUniformLocation(id, name), value.x, value.y, value.z, value.w);
}
void Shader::setMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader) {
    if (useShader) use();
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(GLuint object, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (success) return;
        glGetShaderInfoLog(object, 1024, NULL, infoLog);
        std::cout << "| ERROR::Shader: Compile-time error: Type: " << type << "\n"
                  << infoLog << "\n" << std::endl;
    } else {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (success) return;
        glGetProgramInfoLog(object, 1024, NULL, infoLog);
        std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                  << infoLog << "\n" << std::endl;
    }
}
