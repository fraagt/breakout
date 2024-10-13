#pragma once

#include "glm/glm.hpp"
#include "texture2D.hpp"
#include "sprite_renderer.hpp"

class GameObject {
public:
    glm::vec2 position;
    glm::vec2 size;
    glm::vec2 velocity;
    glm::vec3 color;
    float rotation;
    bool isSolid;
    bool destroyed;
    Texture2D* sprite;

    GameObject();

    virtual void Draw(SpriteRenderer &renderer);
};