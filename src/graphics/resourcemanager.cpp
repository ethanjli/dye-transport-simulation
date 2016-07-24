#include "resourcemanager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <SOIL.h>

std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, Texture> ResourceManager::textures;

Shader &ResourceManager::loadShader(std::string name, std::string vShaderFile,
                                    std::string fShaderFile, std::string gShaderFile) {
    shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return shaders[name];
}

Shader &ResourceManager::getShader(std::string name) {
    return shaders[name];
}

Texture &ResourceManager::loadTexture(std::string name, const GLchar *file, GLboolean alpha) {
    textures[name] = loadTextureFromFile(file, alpha);
    return textures[name];
}

Texture &ResourceManager::getTexture(std::string name) {
    return textures[name];
}

void ResourceManager::clear() {
    for (auto iter : shaders) glDeleteProgram(iter.second.id);
    for (auto iter : textures) glDeleteTextures(1, &iter.second.id);
}

Shader ResourceManager::loadShaderFromFile(std::string vShaderFile,
                                           std::string fShaderFile,
                                           std::string gShaderFile) {
    // Retrieve the vertex/fragment sources from respective file paths
    std::string vertexCode, fragmentCode, geometryCode;
    try {
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // Convert streams into strings
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // Optionally load geometry shader
        if (gShaderFile.size()) {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (std::exception e) {
        std::cout << "ERROR::Shader: Failed to read shader files" << std::endl;
    }
    Shader shader;
    shader.compile(vertexCode, fragmentCode, geometryCode);
    return shader;
}

Texture ResourceManager::loadTextureFromFile(const GLchar *file, GLboolean alpha) {
    Texture texture;
    if (alpha) {
        texture.internalFormat = GL_RGBA;
        texture.imageFormat = GL_RGBA;
    }
    // Load image
    int width, height;
    auto format = texture.imageFormat == GL_RGBA ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB;
    unsigned char *image = SOIL_load_image(file, &width, &height, 0, format);
    texture.generate(width, height, image);
    SOIL_free_image_data(image);
    return texture;
}
