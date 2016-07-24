#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

// Based on the LearnOpenGL 2D Game Rendering Sprites tutorial

#include <map>
#include <string>

#include <GL/glew.h>

#include "texture.h"
#include "shader.h"

// A static singleton that hosts several functions to load Textures and Shaders.
// Each loaded resource is also stored for future reference by string handles.
// All functions and resources are static.
class ResourceManager
{
public:
    // Loads (and generates) a shader program from file, loading vertex, fragment
    // and geometry (optional) shader sources.
    static Shader &loadShader(std::string name, std::string vShaderFile,
                       std::string fShaderFile, std::string gShaderFile = std::string());
    // Retrieves a stored shader
    static Shader &getShader(std::string name);
    // Loads (and generates) a texture from file
    static Texture &loadTexture(std::string name, const GLchar *file, GLboolean alpha);
    // Retrieves a stored texture
    static Texture &getTexture(std::string name);
    // Properly deallocates all loaded resources
    static void clear();

private:
    // Resource storage
    static std::map<std::string, Shader> shaders;
    static std::map<std::string, Texture> textures;

    // Loads and generates a shader from file
    static Shader loadShaderFromFile(std::string vShaderFile, std::string fShaderFile,
                              std::string gShaderFile = std::string());
    // Loads a single texture from file
    static Texture loadTextureFromFile(const GLchar *file, GLboolean alpha);
};

#endif // RESOURCEMANAGER_H
