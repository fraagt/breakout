#pragma once

#include "game_object.hpp"

class BallObject : public GameObject {
public:
    float Radius;
    bool IsStuck;

    glm::vec2 Move(float dt, unsigned int window_width);
    void Reset(glm::vec2  position, glm::vec2 velocity);
};