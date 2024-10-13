#include "resource_manager.hpp"

#include <iostream>
#include <sstream>
#include <fstream>

#include "stb_image.h"

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;


Shader &ResourceManager::LoadShader(const char *vsFilePath,
                                    const char *fsFilePath,
                                    const char *gsFilePath,
                                    const std::string &name) {
    Shaders[name] = loadShaderFromFile(vsFilePath, fsFilePath, gsFilePath);
    return Shaders[name];
}

Shader &ResourceManager::GetShader(const std::string &name) {
    if(Shaders.find(name) == Shaders.end())
        throw std::out_of_range("Shader '" + name + "' not found in ResourceManager.");

    return Shaders[name];
}

Texture2D &ResourceManager::LoadTexture(const char *filePath,
                                        bool alpha,
                                        const std::string &name) {
    auto texture = loadTextureFromFile(filePath, alpha);
    Textures[name] = texture;
    return Textures[name];
}

Texture2D &ResourceManager::GetTexture(const std::string &name) {
    if(Textures.find(name) == Textures.end())
        throw std::out_of_range("Texture '" + name + "' not found in ResourceManager.");

    return Textures[name];
}

void ResourceManager::Clear() {
    // (properly) delete all shaders
    for (const auto &iter: Shaders)
        glDeleteProgram(iter.second.ID);
    // (properly) delete all textures
    for (const auto &iter: Textures)
        glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try {
        // open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr) {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e) {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char *filePath, bool alpha) {
    // create texture object
    Texture2D texture;
    if (alpha) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
//    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filePath, &width, &height, &nrChannels, 3 + static_cast<int>(alpha));
    // now generate texture
    texture.Generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);
    return texture;
}