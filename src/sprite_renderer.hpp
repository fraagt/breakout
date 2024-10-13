#pragma once

#include "shader.hpp"
#include "texture2D.hpp"
#include "mesh.hpp"

class SpriteRenderer {
public:
    Shader *shader;
    Mesh *mesh;

    void Draw(Texture2D &texture,
              glm::vec2 position,
              glm::vec2 size = glm::vec2(10.0f, 10.0f),
              float rotate = 0.0f,
              glm::vec3 color = glm::vec3(1.0f));
};