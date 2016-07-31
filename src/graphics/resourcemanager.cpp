#include "resourcemanager.h"

#include <iostream>
#include <sstream>
#include <fstream>

std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, FluidTexture> ResourceManager::fluidTextures;

Shader &ResourceManager::loadShader(std::string name, std::string vShaderFile,
                                    std::string fShaderFile, std::string gShaderFile) {
    shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return shaders[name];
}

Shader &ResourceManager::getShader(std::string name) {
    return shaders[name];
}

FluidTexture &ResourceManager::loadFluidTexture(std::string name,
                                                const std::shared_ptr<FluidSystem> &fluidSystem) {
    fluidTextures.emplace(std::make_pair(name, fluidSystem));
    fluidTextures.at(name).generate();
    return fluidTextures.at(name);
}

FluidTexture &ResourceManager::getFluidTexture(std::string name) {
    return fluidTextures.at(name);
}

void ResourceManager::clear() {
    for (auto iter : shaders) glDeleteProgram(iter.second.id);
    for (auto iter : fluidTextures) glDeleteTextures(FluidTexture::textures, &iter.second.ids[0]);
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
